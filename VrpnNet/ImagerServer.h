#pragma once

#include "vrpn_Imager.h"
#include "BaseTypes.h"
#include "Connection.h"
#include "ImagerBase.h"

namespace Vrpn
{
	/// <summary>
	/// The VRPN imager server is used to take an image or sequence of images and transmit them to another computer over VRPN.
	/// </summary>
	/// <remarks>
	/// Be careful to only transmit the minimum size of image and minimum number of images that are necessary.  The data streams from the VRPN imager are uncompressed and can easily bog down a network if one is not careful.
	/// Furthermore, the streams are unencrypted and can easily be read by anyone on the network, so don't transmit anything private.
	/// </remarks>
	public ref class ImagerServer: public Vrpn::IVrpnObject
	{
	public:
		ImagerServer(System::String ^name, Vrpn::Connection ^connection, System::Int32 columns, System::Int32 rows);
		ImagerServer(System::String ^name, Vrpn::Connection ^connection, System::Int32 columns, System::Int32 rows, System::Int32 depth);
		~ImagerServer();
		!ImagerServer();

		virtual void Update(); //From IVrpnObject
		virtual Connection^ GetConnection(); //From IVrpnObject
		property System::Boolean MuteWarnings //From IVrpnObject
		{
			virtual void set(System::Boolean);
			virtual System::Boolean get();
		}

		System::Int32 AddChannel(System::String ^channelName);
		System::Int32 AddChannel(System::String ^channelName, ImageBitDepth bitDepth);
		System::Int32 AddChannel(System::String ^channelName, ImageBitDepth bitDepth, System::Single minValue, System::Single maxValue, System::Single scale, System::Single offset);
		System::Boolean SetResolution(System::Int32 columns, System::Int32 rows);
		System::Boolean SetResolution(System::Int32 columns, System::Int32 rows, System::Int32 depth);
		System::Int32 IndexOfChannel(System::String ^channelName);
		void ResendDescription();

		//These look nasty, but all it is really doing is declaring a send image function for each data type (byte, ushort, short, float), and then adding optional parameters for row inverting and multi-slice image support
		System::Boolean SendImage(System::UInt16 channelIndex, System::UInt16 columnStart, System::UInt16 columnStop, System::UInt16 rowStart, System::UInt16 rowStop, System::UInt32 columnStride, System::UInt32 rowStride, array<System::Byte>^ data, 
			[System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt32> bufferOffset, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::Boolean> invertRows, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> height,
			[System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt32> depthStride, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> depthStart, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> depthStop);
		System::Boolean SendImage(System::UInt16 channelIndex, System::UInt16 columnStart, System::UInt16 columnStop, System::UInt16 rowStart, System::UInt16 rowStop, System::UInt32 columnStride, System::UInt32 rowStride, array<System::UInt16>^ data, 
			[System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt32> bufferOffset, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::Boolean> invertRows, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> height,
			[System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt32> depthStride, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> depthStart, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> depthStop);
		System::Boolean SendImage(System::UInt16 channelIndex, System::UInt16 columnStart, System::UInt16 columnStop, System::UInt16 rowStart, System::UInt16 rowStop, System::UInt32 columnStride, System::UInt32 rowStride, array<System::Int16>^ data, 
			[System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt32> bufferOffset, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::Boolean> invertRows, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> height,
			[System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt32> depthStride, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> depthStart, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> depthStop);
		System::Boolean SendImage(System::UInt16 channelIndex, System::UInt16 columnStart, System::UInt16 columnStop, System::UInt16 rowStart, System::UInt16 rowStop, System::UInt32 columnStride, System::UInt32 rowStride, array<System::Single>^ data, 
			[System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt32> bufferOffset, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::Boolean> invertRows, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> height,
			[System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt32> depthStride, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> depthStart, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> depthStop);
		
		//This one is specifically used to send images using the BitmapSource class of WPF.  It is it's own function so users using the data array classes in C# don't have to reference the WPF libraries to disambiguate the overloads.
		System::Boolean SendImageWPF(System::Windows::Media::Imaging::BitmapSource^ image, System::UInt16 redIndex, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> greenIndex, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> blueIndex,
			[System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> alphaIndex, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> columnStart, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> columnStop,
			[System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> rowStart, [System::Runtime::InteropServices::OptionalAttribute] System::Nullable<System::UInt16> rowStop);

		literal System::Int32 MaxChannels = vrpn_IMAGER_MAX_CHANNELS;

	private:
		void Initialize(System::String ^name, Vrpn::Connection ^connection, System::Int32 columns, System::Int32 rows, System::Int32 depth);
		::vrpn_Imager_Server *m_server;
		System::Boolean m_disposed;
		array<ImageBitDepth>^ bitDepths;
		array<System::String^>^ channelNames;
	};
}