#pragma once

#include "vrpn_Imager.h"
#include "BaseTypes.h"
#include "Connection.h"
#include "ImagerBase.h"

namespace Vrpn
{
	public ref class FrameReceivedEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property System::String^ Channel;
		property array<System::Byte>^ Image;
		property System::Boolean IsBoundFrame;
		property System::UInt16 RowMin;
		property System::UInt16 RowMax;
		property System::UInt16 ColumnMin;
		property System::UInt16 ColumnMax;
		property System::UInt16 DepthMin;
		property System::UInt16 DepthMax;
	};

	public ref class FramesDiscardedEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property System::UInt16 DiscardedCount;
		property System::UInt16 RowMin;
		property System::UInt16 RowMax;

	};

	public ref class DescriptionReceivedEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property System::Boolean DimensionsChanged;
		property System::Int32 Rows;
		property System::Int32 Columns;
		property System::Int32 Depth;
	};

	public ref class BeginFrameEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property System::UInt16 RowMin;
		property System::UInt16 RowMax;
		property System::UInt16 ColumnMin;
		property System::UInt16 ColumnMax;
		property System::UInt16 DepthMin;
		property System::UInt16 DepthMax;
	};

	public ref class EndFrameEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property System::UInt16 RowMin;
		property System::UInt16 RowMax;
		property System::UInt16 ColumnMin;
		property System::UInt16 ColumnMax;
		property System::UInt16 DepthMin;
		property System::UInt16 DepthMax;
	};

	public ref class RegionReceivedEventArgs
	{
	public:
		property System::DateTime Time;
		property System::UInt16 ChannelIndex;
		property System::UInt16 RowMin;
		property System::UInt16 RowMax;
		property System::UInt16 ColumnMin;
		property System::UInt16 ColumnMax;
		property System::UInt16 DepthMin;
		property System::UInt16 DepthMax;
		property cli::array<System::Byte>^ RegionData; 
	};

	public delegate void FrameReceivedEventHandler(System::Object ^sender, FrameReceivedEventArgs ^e);
	public delegate void DescriptionReceivedEventHandler(System::Object ^sender, DescriptionReceivedEventArgs ^e);
	public delegate void FramesDiscardedEventHandler(System::Object ^sender, FramesDiscardedEventArgs ^e);
	public delegate void BeginFrameEventHandler(System::Object ^sender, BeginFrameEventArgs ^e);
	public delegate void EndFrameEventHandler(System::Object ^sender, EndFrameEventArgs ^e);
	public delegate void RegionReceivedEventHandler(System::Object ^sender, RegionReceivedEventArgs ^e);

	/// <summary>
	/// The VRPN imager remote receives images transmitted over VRPN.
	/// </summary>
	public ref class ImagerRemote: public Vrpn::IVrpnObject
	{
	public:
		ImagerRemote(System::String ^name);
		ImagerRemote(System::String ^name, Vrpn::Connection ^connection);
		~ImagerRemote();
		!ImagerRemote();

		virtual void Update(); //From IVrpnObject
		virtual Connection^ GetConnection(); //From IVrpnObject
		property System::Boolean MuteWarnings //From IVrpnObject
		{
			virtual void set(System::Boolean);
			virtual System::Boolean get();
		}
		property System::Int32 Rows
		{
			virtual System::Int32 get();
		}
		property System::Int32 Columns
		{
			virtual System::Int32 get();
		}
		property System::Int32 Depth
		{
			virtual System::Int32 get();
		}
		property cli::array<ImagerChannel^> ^Channels
		{
			virtual cli::array<ImagerChannel^>^ get();
		}
		System::Boolean RequestThrottling(System::Int32 requestedFrames);
		System::Boolean BindChannels(System::String^ virtualChannelName, System::String^ redChannel, System::UInt32 redOffset, System::String^ greenChannel, System::UInt32 greenOffset, System::String^ blueChannel, System::UInt32 blueOffset,
			[System::Runtime::InteropServices::OptionalAttribute] System::String^ alphaChannel, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt32> alphaOffset);
		void UnbindChannels();

		/// <summary>
		/// This event is thrown when a full frame of data has been received from the server.
		/// </summary>
		/// <remarks>Depending on if channel binding is in use or not, this may be thrown when an individual channels data has been received or when all the channels specified in the channel binding have received their data.</remarks>
		event FrameReceivedEventHandler^ FrameReceived;
		/// <summary>
		/// This event is thrown when the server notifies the remote that frames have been dropped due to throttling.
		/// </summary>
		event FramesDiscardedEventHandler^ FramesDiscarded;
		/// <summary>
		/// This event is thrown when the server updates the description of the image.
		/// </summary>
		/// <remarks>This is thrown most often when the size of the image changes; however, it can also occur when the bit depth of a channel changes even if the image size remains the same.</remarks>
		event DescriptionReceivedEventHandler^ DescriptionReceived;
		/// <summary>
		/// This event is thrown when the server idicates it is beginning the transmission of a frame.
		/// </summary>
		/// <remarks>This event contains no pixel information, only the boundaries of the frame being sent.</remarks>
		event BeginFrameEventHandler^ BeginFrameReceived;
		/// <summary>
		/// This event is thrown when the server idicates it is ending the transmission of a frame.
		/// </summary>
		/// <remarks>This event contains no pixel information, only the boundaries of the frame that was sent.</remarks>
		event EndFrameEventHandler^ EndFrameReceived;
		/// <summary>
		/// This event is thrown when the server sends a region of the frame.  These regions will automatically be compiled into the full frame sent as part of the FrameReceived event.
		/// </summary>
		/// <remarks>It is recommended that applications use the FrameReceived event instead of this one, unless the application needs to update the subregions immediately upon reception.</remarks>
		event RegionReceivedEventHandler^ RegionReceived;

	private:
		::vrpn_Imager_Remote *m_imager;

		void RegisterHandlers();
		void onDescriptionChanged(void *userData, const timeval info);
		void onDiscardedFrames(void *userData, vrpn_IMAGERDISCARDEDFRAMESCB info);
		void onEndFrame(void *userData, vrpn_IMAGERENDFRAMECB info);
		void onBeginFrame(void *userData, vrpn_IMAGERBEGINFRAMECB info);
		void onRegionReceived(void *userData, vrpn_IMAGERREGIONCB info);
		void setupCombinedFrame();

		System::Runtime::InteropServices::GCHandle gc_descCallback;
		System::Runtime::InteropServices::GCHandle gc_discardCallback;
		System::Runtime::InteropServices::GCHandle gc_endFrameCallback;
		System::Runtime::InteropServices::GCHandle gc_beginFrameCallback;
		System::Runtime::InteropServices::GCHandle gc_regionReceivedCallback;
		System::Boolean m_disposed;
		int rows;
		int columns;
		int depth;
		cli::array<ImagerChannel^> ^channels;
		cli::array<unsigned char*> ^fullFrames;
		cli::array<bool> ^dirtyChannels;
		System::String^ boundChannelName;
		bool hasChannelBinding;
		bool isAlphaBound;
		int redBindIndex;
		int greenBindIndex;
		int blueBindIndex;
		int alphaBindIndex;
		UINT32 redBindOffset;
		UINT32 greenBindOffset;
		UINT32 blueBindOffset;
		UINT32 alphaBindOffset;
		UINT32 colStrideBound;
		cli::array<System::Byte>^ combinedFrame;
		bool redDirty;
		bool greenDirty;
		bool blueDirty;
		bool alphaDirty;
	};
}