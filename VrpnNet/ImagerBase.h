#pragma once

namespace Vrpn
{
	public enum class ImageBitDepth {unsigned8bit, unsigned12in16bit, unsigned16bit, float32bit}; 

	private ref class ImagerUtils
	{
	internal:
		static const char* BitDepthToString(ImageBitDepth depth);
		static ImageBitDepth StringToBitDepth(const char* depthString);
	private:
		ImagerUtils();
	};

	public ref class ImagerChannel
	{
	public:
		System::String ^Name;
		ImageBitDepth BitDepth;
		System::Single MinValue;
		System::Single MaxValue;
		System::Single Offset;
		System::Single Scale;
	};
}