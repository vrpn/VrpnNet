#include "stdafx.h"
#include "ImagerServer.h"
#include <math.h>
#include <algorithm>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace System::Windows::Media;
using namespace System::Windows::Media::Imaging;
using namespace Vrpn;
using namespace std;

/// <summary>
/// Creates a new instance of the VRPN imager server.
/// </summary>
/// <param name="name">The name of the server.</param>
/// <param name="connection">The connection the server should use.</param>
/// <param name="columns">The number of columns in the image stream to be sent (i.e. the image width).</param>
/// <param name="rows">The number of rows in the image stream to be sent (i.e. the image height).</param>
ImagerServer::ImagerServer(System::String ^name, Vrpn::Connection ^connection, int columns, int rows)
{
	Initialize(name, connection, columns, rows, 1);
}

/// <summary>
/// Creates a new instance of the VRPN imager server for a multislice image.
/// </summary>
/// <param name="name">The name of the server.</param>
/// <param name="connection">The connection the server should use.</param>
/// <param name="columns">The number of columns in the image stream to be sent (i.e. the image width).</param>
/// <param name="rows">The number of rows in the image stream to be sent (i.e. the image height).</param>
/// <param name="depth">The number of slices in the image stream to be sent.</param>
ImagerServer::ImagerServer(System::String ^name, Vrpn::Connection ^connection, int columns, int rows, int depth)
{
	Initialize(name, connection, columns, rows, depth);
}

/// <summary>
/// Adds a channel to the imager server.  This functions assumes a channel bit depth of 8 bits.
/// </summary>
/// <param name="channelName">The name of the channel to add.</param>
/// <returns>The index the channel is assigned to.</returns>
int ImagerServer::AddChannel(String ^channelName)
{
	CHECK_DISPOSAL_STATUS();
	IntPtr hName = Marshal::StringToHGlobalAnsi(channelName);
	const char *cName = static_cast<const char*>(hName.ToPointer());
	int channelIndex = m_server->add_channel(cName);

	if (channelIndex >= 0)
	{
		bitDepths[channelIndex] = ImageBitDepth::unsigned8bit;
		channelNames[channelIndex] = channelName;
	}

	return channelIndex;
}

/// <summary>
/// Adds a channel to the imager server with a given channel bit depth.
/// </summary>
/// <param name="channelName">The name of the channel to add.</param>
/// <param name="bitDepth">The bit depth of the new channel.</param>
/// <returns>The index the channel is assigned to.</returns>
int ImagerServer::AddChannel(String ^channelName, ImageBitDepth bitDepth)
{
	CHECK_DISPOSAL_STATUS();
	IntPtr hName = Marshal::StringToHGlobalAnsi(channelName);
	const char *cName = static_cast<const char*>(hName.ToPointer());
	const char *cUnits = ImagerUtils::BitDepthToString(bitDepth);

	int channelIndex = m_server->add_channel(cName, cUnits);

	if (channelIndex >= 0)
	{
		bitDepths[channelIndex] = bitDepth;
		channelNames[channelIndex] = channelName;
	}

	return channelIndex;
}

/// <summary>
/// Adds a channel to the imager server with a given channel bit depth and sets up an automatic interpolation of the input data.
/// </summary>
/// <param name="channelName">The name of the channel to add.</param>
/// <param name="bitDepth">The bit depth of the new channel.</param>
/// <param name="minValue">The minimum value the channel will hold.</param>
/// <param name="maxValue">The maximum value the channel will hold.</param>
/// <param name="scale">The scale to multiple the raw image data by.</param>
/// <param name="offset">How much to offset the interpolation by.</param>
/// <returns>The index the channel is assigned to.</returns>
int ImagerServer::AddChannel(String ^channelName, ImageBitDepth bitDepth, float minValue, float maxValue, float scale, float offset)
{
	CHECK_DISPOSAL_STATUS();
	IntPtr hName = Marshal::StringToHGlobalAnsi(channelName);
	const char *cName = static_cast<const char*>(hName.ToPointer());
	const char *cUnits = ImagerUtils::BitDepthToString(bitDepth);

	int channelIndex = m_server->add_channel(cName, cUnits, minValue, maxValue, scale, offset);

	if (channelIndex >= 0)
	{
		bitDepths[channelIndex] = bitDepth;
		channelNames[channelIndex] = channelName;
	}

	return channelIndex;
}

/// <summary>
/// Sets the resolution of the imager server.  This is only required if the resolution changes after the server is created.
/// </summary>
/// <param name="columns">The number of columns in the image (i.e. the width).</param>
/// <param name="rows">The number of rows in the image (i.e. the height).</param>
/// <returns>Whether or not the change in resolution was successful.</returns>
bool ImagerServer::SetResolution(int columns, int rows)
{
	CHECK_DISPOSAL_STATUS();
	return m_server->set_resolution(columns, rows);
}

/// <summary>
/// Sets the resolution of a multislice imager server.  This is only required if the resolution changes after the server is created.
/// </summary>
/// <param name="columns">The number of columns in the image (i.e. the width).</param>
/// <param name="rows">The number of rows in the image (i.e. the height).</param>
/// <param name="depth">The number of slices in the image.</param>
/// <returns>Whether or not the change in resolution was successful.</returns>
bool ImagerServer::SetResolution(int columns, int rows, int depth)
{
	CHECK_DISPOSAL_STATUS();
	return m_server->set_resolution(columns, rows, depth);
}

/// <summary>
/// Gets the index of a channel from the channel&apos;s name.
/// </summary>
/// <param name="channelName">The name of the channel to lookup.</param>
/// <returns>The index of the channel in the server.</returns>
int ImagerServer::IndexOfChannel(String ^channelName)
{
	int index = -1;

	for (int i = 0; i < m_server->nChannels(); i++)
	{
		if (String::Compare(channelName, channelNames[i]) == 0)
		{
			index = i;
		}
	}

	return index;
}

/// <summary>
/// Sends an image stored in a raw byte array.  If the base type of the image is not a byte (for example, a Gray16 image has a ushort base type), it will convert the array type before sending it.
/// This is not the same as VRPNs default behavior, which would be to omit the 8 least significant bits.
/// </summary>		
/// <param name="channelIndex">The index of the channel on the server to be used to transmit the image data.</param>
/// <param name="columnStart">The first column of the image to transmit.  In the case of transmitting a full image, this value is typically 0.  However, alternate values allow only a portion of the full image array to be sent.</param>
/// <param name="columnStop">The last column of the image to transmit.  In the case of transmitting a full image, this value is typically the image width - 1.</param>
/// <param name="rowStart">The first row of the image to transmit.  This is typically 0.</param>
/// <param name="rowStop">The last row of the image to transmit.  This is typically height - 1.</param>
/// <param name="columnStride">The spacing, in bytes, between adjacent pixels for the current channel.  For a gray image, this is the bytes per pixel, for a color this may be higher (depending on how the color is stored) to allow the code to automatically seperate out the color channels.</param>
/// <param name="rowStride">The width of a full row of pixel data in bytes.</param>
/// <param name="data">The array containing the image data.  It may contain multiple color channels, which this function will separate with the given stride and buffer offset parameters.</param>
/// <param name="bufferOffset">The offset, in bytes, from the beginning of the data array at which the data should start being read.  This is usually 0 or the byte offset of a given color channel.  For example, the green image of an rgb24 image would have an offset of 1.</param>
/// <param name="invertRows">Specifies if the image should be flipped vertically.  Assumed to be false unless specified.</param>
/// <param name="height">The height of the image in pixels.  Only required if invertRows is true or depth parameters are used.</param>
/// <param name="depthStride">The size of a slice in bytes.  Only required when sending a multislice image (aka, a volume).</param>
/// <param name="depthStart">The first slice of a multislice image to send.  Only required when sending a multislice image.</param>
/// <param name="depthStop">The last slice of a multislice image to send.  Only required when sending a multislice image.</param>
/// <returns>Returns true if the transmission was successful, otherwise returns false.  This does not guarantee the transmission was received.</returns>
bool ImagerServer::SendImage(USHORT channelIndex, USHORT columnStart, USHORT columnStop, USHORT rowStart, USHORT rowStop, UINT columnStride, UINT rowStride, array<Byte>^ data, Nullable<UINT32> bufferOffset, Nullable<bool> invertRows, Nullable<USHORT> height, Nullable<UINT> depthStride, Nullable<USHORT> depthStart, Nullable<USHORT> depthStop)
{
	CHECK_DISPOSAL_STATUS();

	//Do all the checking of optional parameters
	bool hasInvertData = false;
	if (invertRows.HasValue)
	{
		if (invertRows.Value)
		{
			if (height.HasValue)
			{
				hasInvertData = true;
			}
			else
			{
				throw gcnew ArgumentNullException("height");
			}
		}
	}
	bool hasDepthData = false;
	if (depthStride.HasValue || depthStart.HasValue || depthStop.HasValue)
	{
		if (depthStride.HasValue && depthStart.HasValue && depthStop.HasValue && hasInvertData)
		{
			hasDepthData = true;
		}
		else
		{
			throw gcnew ArgumentException("To use the depth option invertRows, height, depthStide, depthStart, and depthStop must all have values.");
		}
	}
	int buffOffset = 0;
	if (bufferOffset.HasValue)
	{
		buffOffset = bufferOffset.Value;
	}

	bool success = false;
	if (hasDepthData)
	{
		success = m_server->send_begin_frame(columnStart, columnStop, rowStart, rowStop, depthStart.Value, depthStop.Value);
	}
	else
	{
		success = m_server->send_begin_frame(columnStart, columnStop, rowStart, rowStop);
	}
	m_server->mainloop();

	ImageBitDepth depth = bitDepths[channelIndex];
	UInt16 rowsPerRegion = 0;

	switch(depth)
	{
		case ImageBitDepth::unsigned8bit:
		{
			rowsPerRegion = (UInt16)floor(vrpn_IMAGER_MAX_REGIONu8 / (columnStop - columnStart + 1.0));
			break;
		}
		case ImageBitDepth::unsigned12in16bit:
		case ImageBitDepth::unsigned16bit:
		{
			rowsPerRegion = (UInt16)floor(vrpn_IMAGER_MAX_REGIONu16 / (columnStop - columnStart + 1.0));
			break;
		}
		case ImageBitDepth::float32bit:
		{
			rowsPerRegion = (UInt16)floor(vrpn_IMAGER_MAX_REGIONf32 / (columnStop - columnStart + 1.0));
			break;
		}
	}

	if (rowsPerRegion < 1)
	{
		success = false;
	}

	UInt16 y = rowStart;
	pin_ptr<Byte> pin1 = &data[data->GetLowerBound(0)];
	if (!hasInvertData && !hasDepthData)
	{
		while (success && y < (rowStop + 1))
		{
			if (depth == ImageBitDepth::unsigned8bit)
			{
				success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), pin1 + buffOffset, columnStride, rowStride);
			}
			else if (depth == ImageBitDepth::float32bit)
			{
				success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), (float*)(pin1 + buffOffset), columnStride / 4, rowStride / 4);
			}
			else
			{
				success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), (unsigned short*)(pin1 + buffOffset), columnStride / 2, rowStride / 2);
			}
			m_server->mainloop();
			y += rowsPerRegion;
		}
	}
	else if (hasInvertData && !hasDepthData)
	{
		while (success && y < (rowStop + 1))
		{
			if (depth == ImageBitDepth::unsigned8bit)
			{
				success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), pin1 + buffOffset, columnStride, rowStride, height.Value, invertRows.Value);
			}
			else if (depth == ImageBitDepth::float32bit)
			{
				success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), (float*)(pin1 + buffOffset), columnStride / 4, rowStride / 4, height.Value, invertRows.Value);
			}
			else
			{
				success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), (unsigned short*)(pin1 + buffOffset), columnStride / 2, rowStride / 2, height.Value, invertRows.Value);
			}
			m_server->mainloop();
			y += rowsPerRegion;
		}
	}
	else
	{
		while (success && y < (rowStop + 1))
		{
			if (depth == ImageBitDepth::unsigned8bit)
			{
				success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), pin1 + buffOffset, columnStride, rowStride, height.Value, invertRows.Value, depthStride.Value, depthStart.Value, depthStop.Value);
			}
			else if (depth == ImageBitDepth::unsigned8bit)
			{
				success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), (float*)(pin1 + buffOffset), columnStride / 4, rowStride / 4, height.Value, invertRows.Value, depthStride.Value / 4, depthStart.Value, depthStop.Value);
			}
			else
			{
				success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), (unsigned short*)(pin1 + buffOffset), columnStride / 2, rowStride / 2, height.Value, invertRows.Value, depthStride.Value / 2, depthStart.Value, depthStop.Value);
			}
			m_server->mainloop();
			y += rowsPerRegion;
		}
	}

	if (success)
	{
		if (hasDepthData)
		{
			success = m_server->send_end_frame(columnStart, columnStop, rowStart, rowStop, depthStart.Value, depthStop.Value);
		}
		else
		{
			success = m_server->send_end_frame(columnStart, columnStop, rowStart, rowStop);
		}
		m_server->mainloop();
		m_server->connectionPtr()->mainloop();
	}

	return success;
}

/// <summary>
/// Sends an image stored in an unsigned 16-bit integer array.  Unlike the byte array version of this function, this version will not work with other base data types (i.e. bytes) stored in a ushort array.
/// </summary>		
/// <param name="channelIndex">The index of the channel on the server to be used to transmit the image data.</param>
/// <param name="columnStart">The first column of the image to transmit.  In the case of transmitting a full image, this value is typically 0.  However, alternate values allow only a portion of the full image array to be sent.</param>
/// <param name="columnStop">The last column of the image to transmit.  In the case of transmitting a full image, this value is typically the image width - 1.</param>
/// <param name="rowStart">The first row of the image to transmit.  This is typically 0.</param>
/// <param name="rowStop">The last row of the image to transmit.  This is typically height - 1.</param>
/// <param name="columnStride">The spacing, in bytes, between adjacent pixels for the current channel.  For a gray image, this is the bytes per pixel, for a color this may be higher (depending on how the color is stored) to allow the code to automatically seperate out the color channels.</param>
/// <param name="rowStride">The width of a full row of pixel data in bytes.</param>
/// <param name="data">The array containing the image data.  It may contain multiple color channels, which this function will separate with the given stride and buffer offset parameters.</param>
/// <param name="bufferOffset">The offset, in bytes, from the beginning of the data array at which the data should start being read.  This is usually 0 or the byte offset of a given color channel.  For example, the green image of an rgb24 image would have an offset of 1.</param>
/// <param name="invertRows">Specifies if the image should be flipped vertically.  Assumed to be false unless specified.</param>
/// <param name="height">The height of the image in pixels.  Only required if invertRows is true or depth parameters are used.</param>
/// <param name="depthStride">The size of a slice in bytes.  Only required when sending a multislice image (aka, a volume).</param>
/// <param name="depthStart">The first slice of a multislice image to send.  Only required when sending a multislice image.</param>
/// <param name="depthStop">The last slice of a multislice image to send.  Only required when sending a multislice image.</param>
/// <returns>Returns true if the transmission was successful, otherwise returns false.  This does not guarantee the transmission was received.</returns>
bool ImagerServer::SendImage(USHORT channelIndex, USHORT columnStart, USHORT columnStop, USHORT rowStart, USHORT rowStop, UINT columnStride, UINT rowStride, array<USHORT>^ data, Nullable<UINT32> bufferOffset, Nullable<bool> invertRows, Nullable<USHORT> height, Nullable<UINT> depthStride, Nullable<USHORT> depthStart, Nullable<USHORT> depthStop)
{
	CHECK_DISPOSAL_STATUS();

	ImageBitDepth depth = bitDepths[channelIndex];
	if (depth != ImageBitDepth::unsigned12in16bit && depth != ImageBitDepth::unsigned16bit)
	{
		throw gcnew InvalidOperationException("This function can only transmit on a 16-bit imager channel.");
	}

	//Do all the checking of optional parameters
	bool hasInvertData = false;
	if (invertRows.HasValue)
	{
		if (invertRows.Value)
		{
			if (height.HasValue)
			{
				hasInvertData = true;
			}
			else
			{
				throw gcnew ArgumentNullException("height");
			}
		}
	}
	bool hasDepthData = false;
	if (depthStride.HasValue || depthStart.HasValue || depthStop.HasValue)
	{
		if (depthStride.HasValue && depthStart.HasValue && depthStop.HasValue && hasInvertData)
		{
			hasDepthData = true;
		}
		else
		{
			throw gcnew ArgumentException("To use the depth option invertRows, height, depthStide, depthStart, and depthStop must all have values.");
		}
	}
	int buffOffset = 0;
	if (bufferOffset.HasValue)
	{
		buffOffset = bufferOffset.Value;
	}

	//Send the begin frame message
	bool success = false;
	if (hasDepthData)
	{
		success = m_server->send_begin_frame(columnStart, columnStop, rowStart, rowStop, depthStart.Value, depthStop.Value);
	}
	else
	{
		success = m_server->send_begin_frame(columnStart, columnStop, rowStart, rowStop);
	}
	m_server->mainloop();

	UInt16 rowsPerRegion = 0;

	switch(depth)
	{
		case ImageBitDepth::unsigned8bit:
		{
			rowsPerRegion = (UInt16)floor(vrpn_IMAGER_MAX_REGIONu8 / (columnStop - columnStart + 1.0));
			break;
		}
		case ImageBitDepth::unsigned12in16bit:
		case ImageBitDepth::unsigned16bit:
		{
			rowsPerRegion = (UInt16)floor(vrpn_IMAGER_MAX_REGIONu16 / (columnStop - columnStart + 1.0));
			break;
		}
		case ImageBitDepth::float32bit:
		{
			rowsPerRegion = (UInt16)floor(vrpn_IMAGER_MAX_REGIONf32 / (columnStop - columnStart + 1.0));
			break;
		}
	}

	if (rowsPerRegion < 1)
	{
		success = false;
	}

	//Send all the regions that make up the image
	UInt16 y = rowStart;
	pin_ptr<unsigned short> pin1 = &data[data->GetLowerBound(0)];
	if (!hasInvertData && !hasDepthData)
	{
		while (success && y < (rowStop + 1))
		{		
			success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), pin1 + buffOffset / 2, columnStride / 2, rowStride / 2);
			m_server->mainloop();
			y += rowsPerRegion;
		}
	}
	else if (hasInvertData && !hasDepthData)
	{
		while (success && y < (rowStop + 1))
		{		
			success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), pin1 + buffOffset / 2, columnStride / 2, rowStride / 2, height.Value, invertRows.Value);
			m_server->mainloop();
			y += rowsPerRegion;
		}
	}
	else
	{
		while (success && y < (rowStop + 1))
		{		
			success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop ), (int)(y + rowsPerRegion - 1)), pin1 + buffOffset / 2, columnStride / 2, rowStride / 2, height.Value, invertRows.Value, depthStride.Value / 2, depthStart.Value, depthStop.Value);
			m_server->mainloop();
			y += rowsPerRegion;
		}
	}

	//Send the end of frame message
	if (success)
	{
		if (hasDepthData)
		{
			success = m_server->send_end_frame(columnStart, columnStop, rowStart, rowStop, depthStart.Value, depthStop.Value);
		}
		else
		{
			success = m_server->send_end_frame(columnStart, columnStop, rowStart, rowStop);
		}
		m_server->mainloop();
		m_server->connectionPtr()->mainloop();
	}

	return success;
}

/// <summary>
/// Sends an image stored in a 16-bit integer array.  Unlike the byte array version of this function, this version will not work with other base data types (i.e. bytes) stored in a 16-bit integer array.
/// </summary>		
/// <param name="channelIndex">The index of the channel on the server to be used to transmit the image data.</param>
/// <param name="columnStart">The first column of the image to transmit.  In the case of transmitting a full image, this value is typically 0.  However, alternate values allow only a portion of the full image array to be sent.</param>
/// <param name="columnStop">The last column of the image to transmit.  In the case of transmitting a full image, this value is typically the image width - 1.</param>
/// <param name="rowStart">The first row of the image to transmit.  This is typically 0.</param>
/// <param name="rowStop">The last row of the image to transmit.  This is typically height - 1.</param>
/// <param name="columnStride">The spacing, in bytes, between adjacent pixels for the current channel.  For a gray image, this is the bytes per pixel, for a color this may be higher (depending on how the color is stored) to allow the code to automatically seperate out the color channels.</param>
/// <param name="rowStride">The width of a full row of pixel data in bytes.</param>
/// <param name="data">The array containing the image data.  It may contain multiple color channels, which this function will separate with the given stride and buffer offset parameters.</param>
/// <param name="bufferOffset">The offset, in bytes, from the beginning of the data array at which the data should start being read.  This is usually 0 or the byte offset of a given color channel.  For example, the green image of an rgb24 image would have an offset of 1.</param>
/// <param name="invertRows">Specifies if the image should be flipped vertically.  Assumed to be false unless specified.</param>
/// <param name="height">The height of the image in pixels.  Only required if invertRows is true or depth parameters are used.</param>
/// <param name="depthStride">The size of a slice in bytes.  Only required when sending a multislice image (aka, a volume).</param>
/// <param name="depthStart">The first slice of a multislice image to send.  Only required when sending a multislice image.</param>
/// <param name="depthStop">The last slice of a multislice image to send.  Only required when sending a multislice image.</param>
/// <returns>Returns true if the transmission was successful, otherwise returns false.  This does not guarantee the transmission was received.</returns>
bool ImagerServer::SendImage(USHORT channelIndex, USHORT columnStart, USHORT columnStop, USHORT rowStart, USHORT rowStop, UINT columnStride, UINT rowStride, array<short>^ data, Nullable<UINT32> bufferOffset, Nullable<bool> invertRows, Nullable<USHORT> height, Nullable<UINT> depthStride, Nullable<USHORT> depthStart, Nullable<USHORT> depthStop)
{
	CHECK_DISPOSAL_STATUS();

	ImageBitDepth depth = bitDepths[channelIndex];
	if (depth != ImageBitDepth::unsigned12in16bit && depth != ImageBitDepth::unsigned16bit)
	{
		throw gcnew InvalidOperationException("This function can only transmit on a 16-bit imager channel.");
	}

	//Do all the checking of optional parameters
	bool hasInvertData = false;
	if (invertRows.HasValue)
	{
		if (invertRows.Value)
		{
			if (height.HasValue)
			{
				hasInvertData = true;
			}
			else
			{
				throw gcnew ArgumentNullException("height");
			}
		}
	}
	bool hasDepthData = false;
	if (depthStride.HasValue || depthStart.HasValue || depthStop.HasValue)
	{
		if (depthStride.HasValue && depthStart.HasValue && depthStop.HasValue && hasInvertData)
		{
			hasDepthData = true;
		}
		else
		{
			throw gcnew ArgumentException("To use the depth option invertRows, height, depthStide, depthStart, and depthStop must all have values.");
		}
	}
	int buffOffset = 0;
	if (bufferOffset.HasValue)
	{
		buffOffset = bufferOffset.Value;
	}

	//Send the begin frame message
	bool success = false;
	if (hasDepthData)
	{
		success = m_server->send_begin_frame(columnStart, columnStop, rowStart, rowStop, depthStart.Value, depthStop.Value);
	}
	else
	{
		success = m_server->send_begin_frame(columnStart, columnStop, rowStart, rowStop);
	}
	m_server->mainloop();

	UInt16 rowsPerRegion = 0;

	switch(depth)
	{
		case ImageBitDepth::unsigned8bit:
		{
			rowsPerRegion = (UInt16)floor(vrpn_IMAGER_MAX_REGIONu8 / (columnStop - columnStart + 1.0));
			break;
		}
		case ImageBitDepth::unsigned12in16bit:
		case ImageBitDepth::unsigned16bit:
		{
			rowsPerRegion = (UInt16)floor(vrpn_IMAGER_MAX_REGIONu16 / (columnStop - columnStart + 1.0));
			break;
		}
		case ImageBitDepth::float32bit:
		{
			rowsPerRegion = (UInt16)floor(vrpn_IMAGER_MAX_REGIONf32 / (columnStop - columnStart + 1.0));
			break;
		}
	}

	if (rowsPerRegion < 1)
	{
		success = false;
	}

	//Send all the region messages for the image
	UInt16 y = rowStart;
	pin_ptr<short> pinnedShort = &data[data->GetLowerBound(0)];
	unsigned short* pin1 = (unsigned short*)pinnedShort;
	if (!hasInvertData && !hasDepthData)
	{
		while (success && y < (rowStop + 1))
		{		
			success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), pin1 + buffOffset / 2, columnStride / 2, rowStride / 2);
			m_server->mainloop();
			y += rowsPerRegion;
		}
	}
	else if (hasInvertData && !hasDepthData)
	{
		while (success && y < (rowStop + 1))
		{		
			success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), pin1 + buffOffset / 2, columnStride / 2, rowStride / 2, height.Value, invertRows.Value);
			m_server->mainloop();
			y += rowsPerRegion;
		}
	}
	else
	{
		while (success && y < (rowStop + 1))
		{		
			success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), pin1 + buffOffset / 2, columnStride / 2, rowStride / 2, height.Value, invertRows.Value, depthStride.Value / 2, depthStart.Value, depthStop.Value);
			m_server->mainloop();
			y += rowsPerRegion;
		}
	}

	//Send the end of frame message
	if (success)
	{
		if (hasDepthData)
		{
			success = m_server->send_end_frame(columnStart, columnStop, rowStart, rowStop, depthStart.Value, depthStop.Value);
		}
		else
		{
			success = m_server->send_end_frame(columnStart, columnStop, rowStart, rowStop);
		}
		m_server->mainloop();
		m_server->connectionPtr()->mainloop();
	}

	return success;
}

/// <summary>
/// Sends an image stored in a 32-bit float array.  Unlike the byte array version of this function, this version will not work with other base data types (i.e. bytes) stored in a float array.
/// </summary>		
/// <param name="channelIndex">The index of the channel on the server to be used to transmit the image data.</param>
/// <param name="columnStart">The first column of the image to transmit.  In the case of transmitting a full image, this value is typically 0.  However, alternate values allow only a portion of the full image array to be sent.</param>
/// <param name="columnStop">The last column of the image to transmit.  In the case of transmitting a full image, this value is typically the image width - 1.</param>
/// <param name="rowStart">The first row of the image to transmit.  This is typically 0.</param>
/// <param name="rowStop">The last row of the image to transmit.  This is typically height - 1.</param>
/// <param name="columnStride">The spacing, in bytes, between adjacent pixels for the current channel.  For a gray image, this is the bytes per pixel, for a color this may be higher (depending on how the color is stored) to allow the code to automatically seperate out the color channels.</param>
/// <param name="rowStride">The width of a full row of pixel data in bytes.</param>
/// <param name="data">The array containing the image data.  It may contain multiple color channels, which this function will separate with the given stride and buffer offset parameters.</param>
/// <param name="bufferOffset">The offset, in bytes, from the beginning of the data array at which the data should start being read.  This is usually 0 or the byte offset of a given color channel.  For example, the green image of an rgb24 image would have an offset of 1.</param>
/// <param name="invertRows">Specifies if the image should be flipped vertically.  Assumed to be false unless specified.</param>
/// <param name="height">The height of the image in pixels.  Only required if invertRows is true or depth parameters are used.</param>
/// <param name="depthStride">The size of a slice in bytes.  Only required when sending a multislice image (aka, a volume).</param>
/// <param name="depthStart">The first slice of a multislice image to send.  Only required when sending a multislice image.</param>
/// <param name="depthStop">The last slice of a multislice image to send.  Only required when sending a multislice image.</param>
/// <returns>Returns true if the transmission was successful, otherwise returns false.  This does not guarantee the transmission was received.</returns>
bool ImagerServer::SendImage(USHORT channelIndex, USHORT columnStart, USHORT columnStop, USHORT rowStart, USHORT rowStop, UINT columnStride, UINT rowStride, array<float>^ data, Nullable<UINT32> bufferOffset, Nullable<bool> invertRows, Nullable<USHORT> height, Nullable<UINT> depthStride, Nullable<USHORT> depthStart, Nullable<USHORT> depthStop)
{
	CHECK_DISPOSAL_STATUS();

	ImageBitDepth depth = bitDepths[channelIndex];
	if (depth != ImageBitDepth::float32bit)
	{
		throw gcnew InvalidOperationException("This function can only transmit on a 32-bit imager channel.");
	}

	//Do all the checking of optional parameters
	bool hasInvertData = false;
	if (invertRows.HasValue)
	{
		if (invertRows.Value)
		{
			if (height.HasValue)
			{
				hasInvertData = true;
			}
			else
			{
				throw gcnew ArgumentNullException("height");
			}
		}
	}
	bool hasDepthData = false;
	if (depthStride.HasValue || depthStart.HasValue || depthStop.HasValue)
	{
		if (depthStride.HasValue && depthStart.HasValue && depthStop.HasValue && hasInvertData)
		{
			hasDepthData = true;
		}
		else
		{
			throw gcnew ArgumentException("To use the depth option invertRows, height, depthStide, depthStart, and depthStop must all have values.");
		}
	}
	int buffOffset = 0;
	if (bufferOffset.HasValue)
	{
		buffOffset = bufferOffset.Value;
	}

	//Send the begin frame message
	bool success = false;
	if (hasDepthData)
	{
		success = m_server->send_begin_frame(columnStart, columnStop, rowStart, rowStop, depthStart.Value, depthStop.Value);
	}
	else
	{
		success = m_server->send_begin_frame(columnStart, columnStop, rowStart, rowStop);
	}
	m_server->mainloop();

	UInt16 rowsPerRegion = 0;

	switch(depth)
	{
		case ImageBitDepth::unsigned8bit:
		{
			rowsPerRegion = (UInt16)floor(vrpn_IMAGER_MAX_REGIONu8 / (columnStop - columnStart + 1.0));
			break;
		}
		case ImageBitDepth::unsigned12in16bit:
		case ImageBitDepth::unsigned16bit:
		{
			rowsPerRegion = (UInt16)floor(vrpn_IMAGER_MAX_REGIONu16 / (columnStop - columnStart + 1.0));
			break;
		}
		case ImageBitDepth::float32bit:
		{
			rowsPerRegion = (UInt16)floor(vrpn_IMAGER_MAX_REGIONf32 / (columnStop - columnStart + 1.0));
			break;
		}
	}

	if (rowsPerRegion < 1)
	{
		success = false;
	}

	//Send all the regions messages for the image
	UInt16 y = rowStart;
	pin_ptr<float> pin1 = &data[data->GetLowerBound(0)];
	if (!hasInvertData && !hasDepthData)
	{
		while (success && y < (rowStop + 1))
		{		
			success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), pin1 + buffOffset / 4, columnStride / 4, rowStride / 4);
			m_server->mainloop();
			y += rowsPerRegion;
		}
	}
	else if (hasInvertData && !hasDepthData)
	{
		while (success && y < (rowStop + 1))
		{		
			success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), pin1 + buffOffset / 4, columnStride / 4, rowStride / 4, height.Value, invertRows.Value);
			m_server->mainloop();
			y += rowsPerRegion;
		}
	}
	else
	{
		while (success && y < (rowStop + 1))
		{		
			success = m_server->send_region_using_base_pointer(channelIndex, columnStart, columnStop, y, (UInt16)min((int)(rowStop), (int)(y + rowsPerRegion - 1)), pin1 + buffOffset / 4, columnStride / 4, rowStride / 4, height.Value, invertRows.Value, depthStride.Value / 4, depthStart.Value, depthStop.Value);
			m_server->mainloop();
			y += rowsPerRegion;
		}
	}

	//Send the end frame message, if successful
	if (success)
	{
		if (hasDepthData)
		{
			success = m_server->send_end_frame(columnStart, columnStop, rowStart, rowStop, depthStart.Value, depthStop.Value);
		}
		else
		{
			success = m_server->send_end_frame(columnStart, columnStop, rowStart, rowStop);
		}
		m_server->mainloop();
		m_server->connectionPtr()->mainloop();
	}

	return success;
}

/// <summary>
/// Sends a WPF BitmapSource image via the imager server.  The image will automatically be converted to the bit depth of the server, so make sure the desired bit depth is set first.
/// </summary>		
/// <param name="image">The image to be transmitted.</param>
/// <param name="redIndex">The index of the server channel used to transmit the red channel of the image (or gray channel in the case of a grayscale image).  If the image is grayscale, this is the only index that needs to be specified.</param>
/// <param name="greenIndex">The index of the server channel used to transmit the green channel of the image.  If this is null, the image is assumed to be grayscale.</param>
/// <param name="blueIndex">The index of the server channel used to transmit the blue channel of the image.  If this is null, the image is assumed to be grayscale.</param>
/// <param name="alphaIndex">The index of the server channel used to transmit the alpha channel of the image.  If this is null, no alpha channel will be transmitted.</param>
/// <param name="columnStart">The first column of the image to transmitted.  Assumed to be 0 if not specified.</param>
/// <param name="columnStop">The last column of hte image to be transmitted.  Assummed to be width - 1 if not specified.</param>
/// <param name="rowStart">The first row in the image to be transmitted.  Assumed to be 0 if not specified.</param>
/// <param name="rowStop">The last row of the image to be transmitted.  Assumed to be height - 1 if not specified.</param>
/// <returns>Returns true if the transmission was successful, otherwise returns false.  This does not guarantee the transmission was received.</returns>
bool ImagerServer::SendImageWPF(BitmapSource^ image, USHORT redIndex, Nullable<USHORT> greenIndex, Nullable<USHORT> blueIndex, Nullable<USHORT> alphaIndex, Nullable<USHORT> columnStart, Nullable<USHORT> columnStop, Nullable<USHORT> rowStart, Nullable<USHORT> rowStop)
{
	CHECK_DISPOSAL_STATUS();

	bool isGrayScale = false;
	PixelFormat targetFormat = PixelFormats::Default;
	bool grayChannelValid = false;
	bool RGBChannelsValid = false;
	bool alphaChannelValid = false;

	//Check which channel indices are valid
	if (redIndex >= 0 && redIndex < m_server->nChannels())
	{
		grayChannelValid = true;
		if (greenIndex.HasValue && blueIndex.HasValue)
		{
			if (greenIndex.Value >= 0 && greenIndex.Value < m_server->nChannels() && greenIndex.Value != redIndex)
			{
				if (blueIndex.Value >= 0 && blueIndex.Value < m_server->nChannels() && blueIndex.Value != redIndex && blueIndex.Value != greenIndex.Value)
				{
					RGBChannelsValid = true;

					if (alphaIndex.HasValue)
					{
						if (alphaIndex.Value >= 0 && alphaIndex.Value < m_server->nChannels() && alphaIndex.Value != redIndex && alphaIndex.Value != greenIndex.Value && alphaIndex.Value != blueIndex.Value)
						{
							alphaChannelValid = true;
						}
						else
						{
							throw gcnew ArgumentException("The alpha channel index is invalid.");
						}
					}
				}
				else
				{
					throw gcnew ArgumentException("The blue channel index is invalid.");
				}
			}
			else
			{
				throw gcnew ArgumentException("The green channel index is invalid.");
			}
		}
	}
	else
	{
		throw gcnew ArgumentException("The red channel index is invalid.");
	}

	//If the image is color or color with alpha, make sure the bit depths are consistent
	if (RGBChannelsValid && !alphaChannelValid)
	{
		if (bitDepths[redIndex] != bitDepths[greenIndex.Value] || bitDepths[greenIndex.Value] != bitDepths[blueIndex.Value])
		{
			throw gcnew ArgumentException("The channel bit depths must all be the same for an color image.");
		}
	}
	else if (RGBChannelsValid && alphaChannelValid)
	{
		if (bitDepths[redIndex] != bitDepths[greenIndex.Value] || bitDepths[greenIndex.Value] != bitDepths[blueIndex.Value] || bitDepths[blueIndex.Value] != bitDepths[alphaIndex.Value])
		{
			throw gcnew ArgumentException("The channel bit depths must all be the same for an color image with an alpha channel.");
		}
	}

	//Check if the input image is grayscale
	if (image->Format == PixelFormats::BlackWhite || image->Format == PixelFormats::Gray2 || image->Format == PixelFormats::Gray4 || image->Format == PixelFormats::Gray8 || image->Format == PixelFormats::Gray16 || image->Format == PixelFormats::Gray32Float)
	{
		isGrayScale = true;
	}

	//Find the target image format to transmit
	unsigned int bytesPerPixel = 0;
	unsigned int bytesPerChannel = 0;
	if (RGBChannelsValid && alphaChannelValid)
	{
		if (bitDepths[redIndex] == ImageBitDepth::unsigned8bit)
		{
			targetFormat = PixelFormats::Bgra32;
			bytesPerPixel = 4;
			bytesPerChannel = 1;
		}
		else if (bitDepths[redIndex] == ImageBitDepth::float32bit)
		{
			targetFormat = PixelFormats::Rgba128Float;
			bytesPerPixel = 16;
			bytesPerChannel = 4;
		}
		else
		{
			targetFormat = PixelFormats::Rgba64;
			bytesPerPixel = 8;
			bytesPerChannel = 2;
		}
	}
	else if (RGBChannelsValid && !alphaChannelValid)
	{
		if (bitDepths[redIndex] == ImageBitDepth::unsigned8bit)
		{
			targetFormat = PixelFormats::Rgb24;
			bytesPerPixel = 3;
			bytesPerChannel = 1;
		}
		else if (bitDepths[redIndex] == ImageBitDepth::float32bit)
		{
			targetFormat = PixelFormats::Rgb128Float;
			bytesPerPixel = 16;
			bytesPerChannel = 4;
		}
		else
		{
			targetFormat = PixelFormats::Rgb48;
			bytesPerPixel = 6;
			bytesPerChannel = 2;
		}
	}
	else //Grayscale
	{
		if (bitDepths[redIndex] == ImageBitDepth::unsigned8bit)
		{
			targetFormat = PixelFormats::Gray8;
			bytesPerPixel = 1;
			bytesPerChannel = 1;
		}
		else if (bitDepths[redIndex] == ImageBitDepth::float32bit)
		{
			targetFormat = PixelFormats::Gray32Float;
			bytesPerPixel = 4;
			bytesPerChannel = 4;
		}
		else
		{
			targetFormat = PixelFormats::Gray16;
			bytesPerPixel = 2;
			bytesPerChannel = 2;
		}
	}

	if (targetFormat == PixelFormats::Default)
	{
		throw gcnew InvalidOperationException("The image format to transmit could not be determined.");
	}

	//Convert the image to the format to transmit and get the raw pixel array
	BitmapSource^ convertedImage = gcnew FormatConvertedBitmap(image, targetFormat, nullptr, 0);
	array<Byte>^ rawImage = gcnew array<Byte>(convertedImage->PixelWidth * convertedImage->PixelHeight * bytesPerPixel);
	convertedImage->CopyPixels(rawImage, convertedImage->PixelWidth * bytesPerPixel, 0);

	//Setup the ROI to transmit
	USHORT cStart = 0;
	USHORT cStop = (UInt16)(convertedImage->PixelWidth - 1);
	USHORT rStart = 0;
	USHORT rStop = (UInt16)(convertedImage->PixelHeight - 1);
	if (columnStart.HasValue)
	{
		if (columnStart.Value < convertedImage->PixelWidth)
		{
			cStart = columnStart.Value;
		}
		else
		{
			throw gcnew ArgumentException("The column start must be less than the image width.");
		}
	}
	if (columnStop.HasValue)
	{
		if (columnStop.Value < convertedImage->PixelWidth)
		{
			cStop = columnStop.Value;
		}
		else
		{
			throw gcnew ArgumentException("The column stop must be less than the image width.");
		}
	}
	if (rowStart.HasValue)
	{
		if (rowStart.Value < convertedImage->PixelHeight)
		{
			rStart = rowStart.Value;
		}
		else
		{
			throw gcnew ArgumentException("The row start must be less than the image width.");
		}
	}
	if (rowStop.HasValue)
	{
		if (rowStop.Value < convertedImage->PixelHeight)
		{
			rStop = rowStop.Value;
		}
		else
		{
			throw gcnew ArgumentException("The row stop must be less than the image width.");
		}
	}

	//Transmit the image
	bool success = true;
	if (RGBChannelsValid && alphaChannelValid)
	{
		if (targetFormat == PixelFormats::Bgra32) //This one needs special handling because the channel order is reversed
		{
			success &= SendImage(blueIndex.Value, cStart, cStop, rStart, rStop, bytesPerPixel, convertedImage->PixelWidth * bytesPerPixel, rawImage, 0, Nullable<bool>(), Nullable<USHORT>(), Nullable<UINT32>(), Nullable<USHORT>(), Nullable<USHORT>());
			success &= SendImage(greenIndex.Value, cStart, cStop, rStart, rStop, bytesPerPixel, convertedImage->PixelWidth * bytesPerPixel, rawImage, bytesPerChannel, Nullable<bool>(), Nullable<USHORT>(), Nullable<UINT32>(), Nullable<USHORT>(), Nullable<USHORT>());
			success &= SendImage(redIndex, cStart, cStop, rStart, rStop, bytesPerPixel, convertedImage->PixelWidth * bytesPerPixel, rawImage, 2 * bytesPerChannel, Nullable<bool>(), Nullable<USHORT>(), Nullable<UINT32>(), Nullable<USHORT>(), Nullable<USHORT>());
			success &= SendImage(alphaIndex.Value, cStart, cStop, rStart, rStop, bytesPerPixel, convertedImage->PixelWidth * bytesPerPixel, rawImage, 3 * bytesPerChannel, Nullable<bool>(), Nullable<USHORT>(), Nullable<UINT32>(), Nullable<USHORT>(), Nullable<USHORT>());
		}
		else
		{
			success &= SendImage(redIndex, cStart, cStop, rStart, rStop, bytesPerPixel, convertedImage->PixelWidth * bytesPerPixel, rawImage, 0, Nullable<bool>(), Nullable<USHORT>(), Nullable<UINT32>(), Nullable<USHORT>(), Nullable<USHORT>());
			success &= SendImage(greenIndex.Value, cStart, cStop, rStart, rStop, bytesPerPixel, convertedImage->PixelWidth * bytesPerPixel, rawImage, bytesPerChannel, Nullable<bool>(), Nullable<USHORT>(), Nullable<UINT32>(), Nullable<USHORT>(), Nullable<USHORT>());
			success &= SendImage(blueIndex.Value, cStart, cStop, rStart, rStop, bytesPerPixel, convertedImage->PixelWidth * bytesPerPixel, rawImage, 2 * bytesPerChannel, Nullable<bool>(), Nullable<USHORT>(), Nullable<UINT32>(), Nullable<USHORT>(), Nullable<USHORT>());
			success &= SendImage(alphaIndex.Value, cStart, cStop, rStart, rStop, bytesPerPixel, convertedImage->PixelWidth * bytesPerPixel, rawImage, 3 * bytesPerChannel, Nullable<bool>(), Nullable<USHORT>(), Nullable<UINT32>(), Nullable<USHORT>(), Nullable<USHORT>());
		}
	}
	else if (RGBChannelsValid && !alphaChannelValid)
	{
		success &= SendImage(redIndex, cStart, cStop, rStart, rStop, bytesPerPixel, convertedImage->PixelWidth * bytesPerPixel, rawImage, 0, Nullable<bool>(), Nullable<USHORT>(), Nullable<UINT32>(), Nullable<USHORT>(), Nullable<USHORT>());
		success &= SendImage(greenIndex.Value, cStart, cStop, rStart, rStop, bytesPerPixel, convertedImage->PixelWidth * bytesPerPixel, rawImage, bytesPerChannel, Nullable<bool>(), Nullable<USHORT>(), Nullable<UINT32>(), Nullable<USHORT>(), Nullable<USHORT>());
		success &= SendImage(blueIndex.Value, cStart, cStop, rStart, rStop, bytesPerPixel, convertedImage->PixelWidth * bytesPerPixel, rawImage, 2 * bytesPerChannel, Nullable<bool>(), Nullable<USHORT>(), Nullable<UINT32>(), Nullable<USHORT>(), Nullable<USHORT>());
	}
	else if (grayChannelValid)
	{
		success &= SendImage(redIndex, cStart, cStop, rStart, rStop, bytesPerPixel, convertedImage->PixelWidth * bytesPerPixel, rawImage, 0, Nullable<bool>(), Nullable<USHORT>(), Nullable<UINT32>(), Nullable<USHORT>(), Nullable<USHORT>());
	}
	else
	{
		throw gcnew InvalidOperationException("Cannot transmit the image.");
	}
	
	return success;
}

void ImagerServer::ResendDescription()
{
	CHECK_DISPOSAL_STATUS();
	m_server->send_description();
}

void ImagerServer::Initialize(System::String ^name, Vrpn::Connection ^connection, int columns, int rows, int depth)
{
	IntPtr hName = Marshal::StringToHGlobalAnsi(name);
	const char *cName = static_cast<const char*>(hName.ToPointer());

	m_server = new ::vrpn_Imager_Server(cName, connection->ToPointer(), columns, rows, depth);
	Marshal::FreeHGlobal(hName);

	bitDepths = gcnew array<ImageBitDepth>(vrpn_IMAGER_MAX_CHANNELS);
	channelNames = gcnew array<String^>(vrpn_IMAGER_MAX_CHANNELS);

	m_disposed = false;
}

ImagerServer::!ImagerServer()
{
	delete m_server;
	//Delete anything else here
	m_disposed = true;
}

ImagerServer::~ImagerServer()
{
	this->!ImagerServer();
}

void ImagerServer::Update()
{
	CHECK_DISPOSAL_STATUS();
	m_server->mainloop();
}

Connection^ ImagerServer::GetConnection()
{
	CHECK_DISPOSAL_STATUS();
	return Connection::FromPointer(m_server->connectionPtr());
}

void ImagerServer::MuteWarnings::set(bool shutup)
{
	CHECK_DISPOSAL_STATUS();
	m_server->shutup = shutup;
}

bool ImagerServer::MuteWarnings::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_server->shutup;
}