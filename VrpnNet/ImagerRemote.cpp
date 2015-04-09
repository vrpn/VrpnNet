#include "Stdafx.h"
#include "ImagerRemote.h"

namespace Vrpn 
{
	namespace Internal 
	{
		delegate void DescriptionReceivedCallback(void *userData, const timeval info);
		delegate void FramesDiscardedCallback(void *userData, vrpn_IMAGERDISCARDEDFRAMESCB info);
		delegate void BeginFrameCallback(void *userData, vrpn_IMAGERBEGINFRAMECB info);
		delegate void EndFrameCallback(void *userData, vrpn_IMAGERENDFRAMECB info);
		delegate void RegionReceivedCallback(void *userData, vrpn_IMAGERREGIONCB info);
	}
}

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;
using namespace Vrpn::Internal;

//TODO: DONE) Check all the code to ensure that it will work with a multislice image 
//TODO: DONE) Update the test server to work with multiple transmission methods
//TODO: DONE) Update the test client to use multiple reception methods
//TODO: DONE) Test the code for grayscale, RGB, and RGBA at all bit depths
//TODO: NOT DOING) Test the code with volumes (or maybe just a 2 slice image...)
//TODO: DONE) Add xml documentation to the imager remote code - note in the remarks that channel binding won't work for rgb128 images because it returns a rgb96 image instead.
//TODO: DONE) Add xml documentation to "jane stop this crazy thing"
//TODO: 8) Update the readme
//TODO: 9) Upload code to github

/// <summary>
/// Creates a new instance of the VRPN imager remote.
/// </summary>
/// <param name="name">The name of the VRPN imager server to listen to.</param>
ImagerRemote::ImagerRemote(String ^name)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);

	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());
	m_imager = new ::vrpn_Imager_Remote(ansiName);

	Marshal::FreeHGlobal(hAnsiName);

	fullFrames = gcnew array<unsigned char*>(0);
	dirtyChannels = gcnew array<bool>(0);

	channels = gcnew array<ImagerChannel^>(0);
	RegisterHandlers();

	m_disposed = false;
}

/// <summary>
/// Creates a new instance of the VRPN imager remote with a given VRPN connection.
/// </summary>
/// <param name="name">The name of the VRPN imager server to listen to.</param>
/// <param name="connection">The VRPN connection to use for the imager remote.</param>
ImagerRemote::ImagerRemote(String ^name, Vrpn::Connection ^connection)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);

	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());
	m_imager = new ::vrpn_Imager_Remote(ansiName, connection->ToPointer());

	Marshal::FreeHGlobal(hAnsiName);

	fullFrames = gcnew array<unsigned char*>(0);
	dirtyChannels = gcnew array<bool>(0);

	channels = gcnew array<ImagerChannel^>(0);
	RegisterHandlers();

	m_disposed = false;
}

ImagerRemote::!ImagerRemote()
{
	delete m_imager;

	//Cleanup the frames array
	//Note: fullFrames itself doesn't need to be deleted since it is managed, just the unmanged arrays it is holding
	for (int i = 0; i < fullFrames->Length; i++)
	{
		delete[] fullFrames[i];
	}

	//Cleanup callbacks
	gc_descCallback.Free();
	gc_discardCallback.Free();
	gc_endFrameCallback.Free();
	gc_beginFrameCallback.Free();
	gc_regionReceivedCallback.Free();
	m_disposed = true;
}

ImagerRemote::~ImagerRemote()
{
	this->!ImagerRemote();
}

void ImagerRemote::Update()
{
	CHECK_DISPOSAL_STATUS();
	m_imager->mainloop();
}

void ImagerRemote::MuteWarnings::set(Boolean shutup)
{
	CHECK_DISPOSAL_STATUS();
	m_imager->shutup = shutup;
}

Boolean ImagerRemote::MuteWarnings::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_imager->shutup;
}

Int32 ImagerRemote::Rows::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_imager->nRows();
}

Int32 ImagerRemote::Columns::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_imager->nCols();
}

Int32 ImagerRemote::Depth::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_imager->nDepth();
}

cli::array<ImagerChannel^>^ ImagerRemote::Channels::get()
{
	CHECK_DISPOSAL_STATUS();
	return channels;
}

Connection^ ImagerRemote::GetConnection()
{
	CHECK_DISPOSAL_STATUS();
	return Connection::FromPointer(m_imager->connectionPtr());
}

/// <summary>
/// Requests that the server limit the number of frames it sends to the remote.
/// </summary>
/// <param name="requestedFrames">The number of frames for the server to send before stopping.  A value of -1 indicates the server should send continuously.</param>
/// <returns>True if the request was sucessful, otherwise false.</returns>
Boolean ImagerRemote::RequestThrottling(int requestedFrames)
{
	CHECK_DISPOSAL_STATUS();
	return m_imager->throttle_sender(requestedFrames);
}

void ImagerRemote::RegisterHandlers()
{
	//Description received handler
	DescriptionReceivedCallback ^descCB = gcnew DescriptionReceivedCallback(this, &ImagerRemote::onDescriptionChanged);
	gc_descCallback = GCHandle::Alloc(descCB);
	IntPtr pDescRec = Marshal::GetFunctionPointerForDelegate(descCB);
	vrpn_IMAGERDESCRIPTIONHANDLER pDescHandler = static_cast<vrpn_IMAGERDESCRIPTIONHANDLER>(pDescRec.ToPointer());
	m_imager->register_description_handler(0, pDescHandler);

	//Discarded frames info received handler
	FramesDiscardedCallback ^discardCB = gcnew FramesDiscardedCallback(this, &ImagerRemote::onDiscardedFrames);
	gc_discardCallback = GCHandle::Alloc(discardCB);
	IntPtr pDiscardFrames = Marshal::GetFunctionPointerForDelegate(discardCB);
	vrpn_IMAGERDISCARDEDFRAMESHANDLER pDiscardHandler = static_cast<vrpn_IMAGERDISCARDEDFRAMESHANDLER>(pDiscardFrames.ToPointer());
	m_imager->register_discarded_frames_handler(0, pDiscardHandler);

	//Begin frame handler
	BeginFrameCallback ^beginFrameCB = gcnew BeginFrameCallback(this, &ImagerRemote::onBeginFrame);
	gc_beginFrameCallback = GCHandle::Alloc(beginFrameCB);
	IntPtr pBeginFrame = Marshal::GetFunctionPointerForDelegate(beginFrameCB);
	vrpn_IMAGERBEGINFRAMEHANDLER pBeginFrameHandler = static_cast<vrpn_IMAGERBEGINFRAMEHANDLER>(pBeginFrame.ToPointer());
	m_imager->register_begin_frame_handler(0, pBeginFrameHandler);

	//End frame handler
	EndFrameCallback ^endFrameCB = gcnew EndFrameCallback(this, &ImagerRemote::onEndFrame);
	gc_endFrameCallback = GCHandle::Alloc(endFrameCB);
	IntPtr pEndFrame = Marshal::GetFunctionPointerForDelegate(endFrameCB);
	vrpn_IMAGERENDFRAMEHANDLER pEndFrameHandler = static_cast<vrpn_IMAGERENDFRAMEHANDLER>(pEndFrame.ToPointer());
	m_imager->register_end_frame_handler(0, pEndFrameHandler);

	//Region Received Handler
	RegionReceivedCallback ^regionReceivedCB = gcnew RegionReceivedCallback(this, &ImagerRemote::onRegionReceived);
	gc_regionReceivedCallback = GCHandle::Alloc(regionReceivedCB);
	IntPtr pRegionReceived = Marshal::GetFunctionPointerForDelegate(regionReceivedCB);
	vrpn_IMAGERREGIONHANDLER pRegionReceivedHandler = static_cast<vrpn_IMAGERREGIONHANDLER>(pRegionReceived.ToPointer());
	m_imager->register_region_handler(0, pRegionReceivedHandler);
}

void ImagerRemote::onDiscardedFrames(void *, vrpn_IMAGERDISCARDEDFRAMESCB info)
{
	FramesDiscardedEventArgs ^e = gcnew FramesDiscardedEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->DiscardedCount = info.count;

	FramesDiscarded(this, e);
}

void ImagerRemote::onDescriptionChanged(void *, const timeval info)
{
	//Copy the required data to the event args
	DescriptionReceivedEventArgs ^e = gcnew DescriptionReceivedEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info);
	e->DimensionsChanged = false;
	if (m_imager->nRows() != rows)
	{
		e->DimensionsChanged = true;
		rows = m_imager->nRows();
	}
	if (m_imager->nCols() != columns)
	{
		e->DimensionsChanged = true;
		columns = m_imager->nCols();
	}
	if (m_imager->nDepth() != depth)
	{
		e->DimensionsChanged = true;
		depth = m_imager->nDepth();
	}
	e->Rows = rows;
	e->Columns = columns;
	e->Depth = depth;

	//Update the channels array
	channels = gcnew array<ImagerChannel^>(m_imager->nChannels());

	//Update the frames array
	for (int i = 0; i < fullFrames->Length; i++)
	{
		delete[] fullFrames[i];
	}
	fullFrames = gcnew array<unsigned char*>(m_imager->nChannels());
	dirtyChannels = gcnew array<bool>(m_imager->nChannels());

	for (int i = 0; i < m_imager->nChannels(); i++)
	{
		ImagerChannel^ channelTemp = gcnew ImagerChannel();
		channelTemp->Name = gcnew String(m_imager->channel(i)->name);
		channelTemp->BitDepth = ImagerUtils::StringToBitDepth(m_imager->channel(i)->units);
		channelTemp->MinValue = m_imager->channel(i)->minVal;
		channelTemp->MaxValue = m_imager->channel(i)->maxVal;
		channelTemp->Offset = m_imager->channel(i)->offset;
		channelTemp->Scale = m_imager->channel(i)->scale;
		channels[i] = channelTemp;

		int bytesPerPixel = 2;
		if (channelTemp->BitDepth == ImageBitDepth::unsigned8bit)
		{
			bytesPerPixel = 1;
		}
		else if (channelTemp->BitDepth == ImageBitDepth::float32bit)
		{
			bytesPerPixel = 4;
		}
		fullFrames[i] = new unsigned char[rows * columns * depth * bytesPerPixel];
		dirtyChannels[i] = false;
	}

	//TODO: The channel bindings need to update if the description changes

	DescriptionReceived(this, e);
}

void ImagerRemote::onBeginFrame(void *, vrpn_IMAGERBEGINFRAMECB info)
{
	BeginFrameEventArgs ^e = gcnew BeginFrameEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->RowMin = info.rMin;
	e->RowMax = info.rMax;
	e->ColumnMin = info.cMin;
	e->ColumnMax = info.cMax;
	e->DepthMin = info.dMin;
	e->DepthMax = info.dMax;

	BeginFrameReceived(this, e);
}

void ImagerRemote::onEndFrame(void *, vrpn_IMAGERENDFRAMECB info)
{
	//Setup the end frame event args
	EndFrameEventArgs ^e = gcnew EndFrameEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->RowMin = info.rMin;
	e->RowMax = info.rMax;
	e->ColumnMin = info.cMin;
	e->ColumnMax = info.cMax;
	e->DepthMin = info.dMin;
	e->DepthMax = info.dMax;

	//Throw the end of frame event
	EndFrameReceived(this, e);

	//Figure out if a full frame has been received and throw the full frame event if needed
	if (hasChannelBinding)
	{
		if (redDirty && greenDirty && blueDirty)
		{
			if (isAlphaBound)
			{
				if (alphaDirty)
				{
					int bytesPerPixel = 2;
					if (channels[redBindIndex]->BitDepth == ImageBitDepth::unsigned8bit)
					{
						bytesPerPixel = 1;
					}
					else if (channels[redBindIndex]->BitDepth == ImageBitDepth::float32bit)
					{
						bytesPerPixel = 4;
					}

					//Setup the event args data
					FrameReceivedEventArgs^ eFull = gcnew FrameReceivedEventArgs();
					eFull->Channel = boundChannelName;
					eFull->IsBoundFrame = true;
					eFull->Time = VrpnUtils::ConvertTimeval(info.msg_time);
					eFull->RowMin = info.rMin;
					eFull->RowMax = info.rMax;
					eFull->ColumnMin = info.cMin;
					eFull->ColumnMax = info.cMax;
					eFull->DepthMin = info.dMin;
					eFull->DepthMax = info.dMax;

					//Copy the image data to a managed array and pass it to the event args
					array<Byte>^ imageData = gcnew array<Byte>(combinedFrame->Length);
					cli::pin_ptr<Byte> pinPtrDst = &imageData[imageData->GetLowerBound(0)];
					pin_ptr<Byte> pinPtrSrc = &combinedFrame[combinedFrame->GetLowerBound(0)];
					memcpy(pinPtrDst, pinPtrSrc, combinedFrame->Length);
					eFull->Image = imageData;

					FrameReceived(this, eFull);

					redDirty = false;
					greenDirty = false;
					blueDirty = false;
					alphaDirty = false;
				}
			}
			else
			{
				//Setup the event args data
				FrameReceivedEventArgs^ eFull = gcnew FrameReceivedEventArgs();
				eFull->Channel = boundChannelName;
				eFull->IsBoundFrame = true;
				eFull->Time = VrpnUtils::ConvertTimeval(info.msg_time);
				eFull->RowMin = info.rMin;
				eFull->RowMax = info.rMax;
				eFull->ColumnMin = info.cMin;
				eFull->ColumnMax = info.cMax;
				eFull->DepthMin = info.dMin;
				eFull->DepthMax = info.dMax;

				//Copy the image data to a managed array and pass it to the event args
				array<Byte>^ imageData = gcnew array<Byte>(combinedFrame->Length);
				pin_ptr<Byte> pinPtrDst = &imageData[imageData->GetLowerBound(0)];
				pin_ptr<Byte> pinPtrSrc = &combinedFrame[combinedFrame->GetLowerBound(0)];
				memcpy(pinPtrDst, pinPtrSrc, combinedFrame->Length);
				eFull->Image = imageData;

				FrameReceived(this, eFull);

				redDirty = false;
				greenDirty = false;
				blueDirty = false;
			}
		}
	}
	else
	{
		for (int i = 0; i < channels->Length; i++)
		{
			if (dirtyChannels[i])
			{
				int bytesPerPixel = 2;

				if (channels[i]->BitDepth == ImageBitDepth::unsigned8bit)
				{
					bytesPerPixel = 1;
				}
				else if (channels[i]->BitDepth == ImageBitDepth::float32bit)
				{
					bytesPerPixel = 4;
				}

				//Setup the event args data
				FrameReceivedEventArgs^ eFull = gcnew FrameReceivedEventArgs();
				eFull->Channel = channels[i]->Name;
				eFull->IsBoundFrame = false;
				eFull->Time = VrpnUtils::ConvertTimeval(info.msg_time);
				eFull->RowMin = info.rMin;
				eFull->RowMax = info.rMax;
				eFull->ColumnMin = info.cMin;
				eFull->ColumnMax = info.cMax;
				eFull->DepthMin = info.dMin;
				eFull->DepthMax = info.dMax;

				//Copy the image data to a managed array and pass it to the event args
				unsigned int byteCount = rows * columns * depth * bytesPerPixel;
				array<Byte>^ imageData = gcnew array<Byte>(byteCount);
				pin_ptr<Byte> pinPtr = &imageData[imageData->GetLowerBound(0)];
				memcpy(pinPtr, &(fullFrames[i])[0], byteCount);
				eFull->Image = imageData;

				FrameReceived(this, eFull);

				dirtyChannels[i] = false;
			}
		}
	}
}

void ImagerRemote::onRegionReceived(void *, vrpn_IMAGERREGIONCB info)
{
	//Get the bit depth data for the region
	ImageBitDepth bitDepth = ImagerUtils::StringToBitDepth(m_imager->channel(info.region->d_chanIndex)->units);
	int bytesPerPixel = 2;
	if (bitDepth == ImageBitDepth::unsigned8bit)
	{
		bytesPerPixel = 1;
	}
	else if(bitDepth == ImageBitDepth::float32bit)
	{
		bytesPerPixel = 4;
	}

	//Add data to full frame array
	if (bitDepth == ImageBitDepth::unsigned8bit)
	{
		info.region->decode_unscaled_region_using_base_pointer(fullFrames[info.region->d_chanIndex], 1, columns, rows * columns);
	}
	else if (bitDepth == ImageBitDepth::unsigned12in16bit || bitDepth == ImageBitDepth::unsigned16bit)
	{
		info.region->decode_unscaled_region_using_base_pointer((unsigned short*)fullFrames[info.region->d_chanIndex], 1, columns, rows * columns);
	}
	else
	{		
		info.region->decode_unscaled_region_using_base_pointer((float*)fullFrames[info.region->d_chanIndex], 1, columns, rows * columns);
	}
	dirtyChannels[info.region->d_chanIndex] = true; //Mark the channel as dirty so we know to pass the data along when the frame is done (yes, keeping track of which channels are dirty is a bit of a hack, but without changing VRPN, this is how we have to do it)

	//Add the data to the combined frame array, if frame binding is on
	if (hasChannelBinding)
	{
		if (info.region->d_chanIndex == redBindIndex)
		{
			pin_ptr<Byte> pinPtr = &combinedFrame[combinedFrame->GetLowerBound(0)];
			if (bitDepth == ImageBitDepth::unsigned8bit)
			{
				info.region->decode_unscaled_region_using_base_pointer(pinPtr + redBindOffset, colStrideBound, columns * colStrideBound, rows * columns * colStrideBound);
			}
			else if (bitDepth == ImageBitDepth::unsigned12in16bit || bitDepth == ImageBitDepth::unsigned16bit)
			{
				info.region->decode_unscaled_region_using_base_pointer((unsigned short*)(pinPtr + redBindOffset), colStrideBound, columns * colStrideBound, rows * columns * colStrideBound);
			}
			else
			{
				info.region->decode_unscaled_region_using_base_pointer((float*)(pinPtr + redBindOffset), colStrideBound, columns * colStrideBound, rows * columns * colStrideBound);
			}
			redDirty = true;
		}
		else if (info.region->d_chanIndex == greenBindIndex)
		{
			pin_ptr<Byte> pinPtr = &combinedFrame[combinedFrame->GetLowerBound(0)];
			if (bitDepth == ImageBitDepth::unsigned8bit)
			{
				info.region->decode_unscaled_region_using_base_pointer(pinPtr + greenBindOffset, colStrideBound, columns * colStrideBound, rows * columns * colStrideBound);
			}
			else if (bitDepth == ImageBitDepth::unsigned12in16bit || bitDepth == ImageBitDepth::unsigned16bit)
			{
				info.region->decode_unscaled_region_using_base_pointer((unsigned short*)(pinPtr + greenBindOffset), colStrideBound, columns * colStrideBound, rows * columns * colStrideBound);
			}
			else
			{
				info.region->decode_unscaled_region_using_base_pointer((float*)(pinPtr + greenBindOffset), colStrideBound, columns * colStrideBound, rows * columns * colStrideBound);
			}
			greenDirty = true;
		}
		else if (info.region->d_chanIndex == blueBindIndex)
		{
			pin_ptr<Byte> pinPtr = &combinedFrame[combinedFrame->GetLowerBound(0)];
			if (bitDepth == ImageBitDepth::unsigned8bit)
			{
				info.region->decode_unscaled_region_using_base_pointer(pinPtr + blueBindOffset, colStrideBound, columns * colStrideBound, rows * columns * colStrideBound);
			}
			else if (bitDepth == ImageBitDepth::unsigned12in16bit || bitDepth == ImageBitDepth::unsigned16bit)
			{
				info.region->decode_unscaled_region_using_base_pointer((unsigned short*)(pinPtr + blueBindOffset), colStrideBound, columns * colStrideBound, rows * columns * colStrideBound);
			}
			else
			{
				info.region->decode_unscaled_region_using_base_pointer((float*)(pinPtr + blueBindOffset), colStrideBound, columns * colStrideBound, rows * columns * colStrideBound);
			}
			blueDirty = true;
		}
		else if (isAlphaBound)
		{
			if (info.region->d_chanIndex == alphaBindIndex)
			{
				pin_ptr<Byte> pinPtr = &combinedFrame[combinedFrame->GetLowerBound(0)];
				if (bitDepth == ImageBitDepth::unsigned8bit)
				{
					info.region->decode_unscaled_region_using_base_pointer(pinPtr + alphaBindOffset, colStrideBound, columns * colStrideBound, rows * columns * colStrideBound);
				}
				else if (bitDepth == ImageBitDepth::unsigned12in16bit || bitDepth == ImageBitDepth::unsigned16bit)
				{
					info.region->decode_unscaled_region_using_base_pointer((unsigned short*)(pinPtr + alphaBindOffset), colStrideBound, columns * colStrideBound, rows * columns * colStrideBound);
				}
				else
				{
					info.region->decode_unscaled_region_using_base_pointer((float*)(pinPtr + alphaBindOffset), colStrideBound, columns * colStrideBound, rows * columns * colStrideBound);
				}
				alphaDirty = true;
			}
		}
	}

	//Setup the data array for transmitting just the region
	int arraySize =  info.region->getNumVals();
	array<Byte>^ regionData = gcnew array<Byte>(bytesPerPixel * arraySize); //Set the size

	if (bitDepth == ImageBitDepth::unsigned8bit)
	{
		//TODO: Support depth
		pin_ptr<Byte> pinPtr = &regionData[regionData->GetLowerBound(0)];
		unsigned int offset = info.region->d_dMin * rows * columns + info.region->d_rMin * columns + info.region->d_cMin;
		memcpy(pinPtr, &(fullFrames[info.region->d_chanIndex])[offset], arraySize);
	}
	else if (bitDepth == ImageBitDepth::unsigned16bit || bitDepth == ImageBitDepth::unsigned12in16bit)
	{
		pin_ptr<Byte> pinPtr = &regionData[regionData->GetLowerBound(0)];
		unsigned int offset = (info.region->d_dMin * rows * columns + info.region->d_rMin * columns + info.region->d_cMin) * bytesPerPixel;
		memcpy(pinPtr, &(fullFrames[info.region->d_chanIndex])[offset], bytesPerPixel * arraySize);
	}
	else
	{
		pin_ptr<Byte> pinPtr = &regionData[regionData->GetLowerBound(0)];
		unsigned int offset = (info.region->d_dMin * rows * columns + info.region->d_rMin * columns + info.region->d_cMin) * bytesPerPixel;
		memcpy(pinPtr, &(fullFrames[info.region->d_chanIndex])[offset], bytesPerPixel * arraySize);
	}

	//Transmit raw data for this region
	RegionReceivedEventArgs ^e = gcnew RegionReceivedEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->ChannelIndex = info.region->d_chanIndex;
	e->RowMin = info.region->d_rMin;
	e->RowMax = info.region->d_rMax;
	e->ColumnMin = info.region->d_cMin;
	e->ColumnMax = info.region->d_cMax;
	e->DepthMin = info.region->d_dMin;
	e->DepthMax = info.region->d_dMax;
	e->RegionData = regionData;

	RegionReceived(this, e);
}

/// <summary>
/// Tells the remote that it should combine the data frome multiple imager channels into one data array and fire a FrameReceived event when the whole array has been filled.
/// </summary>
/// <param name="virtualChannelName">The name the remote should call the virtual image channel.  Note, this channel is virtual and only exists in the imager remote. It does not appear in the list of imager channels.</param>
/// <param name="redChannel">The name of the imager channel to pull the data for the red channel from.</param>
/// <param name="redOffset">The offset, in bytes, from the beginning of the pixel to where the red channel information is located.</param>
/// <param name="greenChannel">The name of the imager channel to pull the data for the green channel from.</param>
/// <param name="greenOffset">The offset, in bytes, from the beginning of the pixel to where the green channel information is located.</param>
/// <param name="blueChannel">The name of the imager channel to pull the data for the blue channel from.</param>
/// <param name="blueOffset">The offset, in bytes, from the beginning of the pixel to where the blue channel information is located.</param>
/// <param name="alphaChannel">The name of the imager channel to pull the data for the optional alpha channel from.  If no value is specified, no alpha value will be bound.</param>
/// <param name="alphaOffset">The offset, in bytes, from the beginning of the pixel to where the alpha channel information is located.  This parameter is required if the alphaChannel paramter has a value.</param>
/// <returns>True if the channel binding was setup successfully, otherwise false.</returns>
/// <remarks>Using channel binding causes one FrameReceived event to be thrown once all channels of the image have delivered their data.  The FrameReceived events for the individual channels will not be thrown.</remarks>
Boolean ImagerRemote::BindChannels(String^ virtualChannelName, String^ redChannel, UInt32 redOffset, String^ greenChannel, UInt32 greenOffset, String^ blueChannel, UInt32 blueOffset, String^ alphaChannel, Nullable<UInt32> alphaOffset)
{
	Boolean success = true;
	int tempRed = -1;
	int tempGreen = -1;
	int tempBlue = -1;
	int tempAlpha = -1;

	if (alphaChannel != nullptr && alphaOffset.HasValue)
	{
		for (int i = 0; i < channels->Length; i++)
		{
			if (redChannel == channels[i]->Name)
			{
				tempRed = i;
			}
			if (greenChannel == channels[i]->Name)
			{
				tempGreen = i;
			}
			if (blueChannel == channels[i]->Name)
			{
				tempBlue = i;
			}
			if (alphaChannel == channels[i]->Name)
			{
				tempAlpha = i;
			}
		}

		if (tempRed >= 0 && tempGreen >= 0 && tempBlue >= 0 && tempAlpha >= 0)
		{			
			isAlphaBound = true;
			redBindIndex = tempRed;
			greenBindIndex = tempGreen;
			blueBindIndex = tempBlue;
			alphaBindIndex = tempAlpha;
			redBindOffset = redOffset;
			greenBindOffset = greenOffset;
			blueBindOffset = blueOffset;
			alphaBindOffset = alphaOffset.Value;
			boundChannelName = virtualChannelName;
			colStrideBound = 4;
			redDirty = false;
			greenDirty = false;
			blueDirty = false;
			alphaDirty = false;
			setupCombinedFrame();
			hasChannelBinding = true;
		}
		else
		{
			success = false;
		}
	}
	else
	{
		for (int i = 0; i < channels->Length; i++)
		{
			if (redChannel == channels[i]->Name)
			{
				tempRed = i;
			}
			if (greenChannel == channels[i]->Name)
			{
				tempGreen = i;
			}
			if (blueChannel == channels[i]->Name)
			{
				tempBlue = i;
			}
		}

		if (tempRed >= 0 && tempGreen >= 0 && tempBlue >= 0)
		{			
			isAlphaBound = false;
			redBindIndex = tempRed;
			greenBindIndex = tempGreen;
			blueBindIndex = tempBlue;
			redBindOffset = redOffset;
			greenBindOffset = greenOffset;
			blueBindOffset = blueOffset;
			boundChannelName = virtualChannelName;
			colStrideBound = 3;
			redDirty = false;
			greenDirty = false;
			blueDirty = false;
			alphaDirty = false;
			setupCombinedFrame();
			hasChannelBinding = true;
		}
		else
		{
			success = false;
		}
	}

	return success;
}

/// <summary>
/// Clears all channel bindings from the imager remote.  This will cause the FrameReceived event to be thrown whenever an individual channel has received all its data for a single frame.
/// </summary>
void ImagerRemote::UnbindChannels()
{
	hasChannelBinding = false;
	isAlphaBound = false;
	redBindIndex = -1;
	greenBindIndex = -1;
	blueBindIndex = -1;
	alphaBindIndex = -1;
	redBindOffset = 0;
	greenBindOffset = 0;
	blueBindOffset = 0;
	alphaBindOffset = 0;
	boundChannelName = "";
	colStrideBound = 1;
	redDirty = false;
	greenDirty = false;
	blueDirty = false;
	alphaDirty = false;
	combinedFrame = nullptr;
}

void ImagerRemote::setupCombinedFrame()
{
	int bytesPerPixel = 0;

	//Get the bit depth of the red channel
	if (channels[redBindIndex]->BitDepth == ImageBitDepth::unsigned8bit)
	{
		bytesPerPixel += 1;
	}
	else if (channels[redBindIndex]->BitDepth == ImageBitDepth::float32bit)
	{
		bytesPerPixel += 4;
	}
	else
	{
		bytesPerPixel += 2;
	}

	//Get the bit depth of the green channel
	if (channels[greenBindIndex]->BitDepth == ImageBitDepth::unsigned8bit)
	{
		bytesPerPixel += 1;
	}
	else if (channels[greenBindIndex]->BitDepth == ImageBitDepth::float32bit)
	{
		bytesPerPixel += 4;
	}
	else
	{
		bytesPerPixel += 2;
	}

	//Get the bit depth of the blue channel
	if (channels[blueBindIndex]->BitDepth == ImageBitDepth::unsigned8bit)
	{
		bytesPerPixel += 1;
	}
	else if (channels[blueBindIndex]->BitDepth == ImageBitDepth::float32bit)
	{
		bytesPerPixel += 4;
	}
	else
	{
		bytesPerPixel += 2;
	}

	//Get the bit depth of the alpha channel, if it exists
	if (isAlphaBound)
	{
		if (channels[alphaBindIndex]->BitDepth == ImageBitDepth::unsigned8bit)
		{
			bytesPerPixel += 1;
		}
		else if (channels[alphaBindIndex]->BitDepth == ImageBitDepth::float32bit)
		{
			bytesPerPixel += 4;
		}
		else
		{
			bytesPerPixel += 2;
		}
	}

	combinedFrame = gcnew array<Byte>(rows * columns * depth * bytesPerPixel);
}