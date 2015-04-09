#include "Stdafx.h"
#include "ImagerBase.h"
#include <algorithm>

using namespace Vrpn;

const char* ImagerUtils::BitDepthToString(ImageBitDepth depth)
{
	switch (depth)
	{
		case ImageBitDepth::unsigned8bit: return "unsigned8";
		case ImageBitDepth::unsigned12in16bit: return "unsigned12in16";
		case ImageBitDepth::unsigned16bit: return "unsigned16";
		case ImageBitDepth::float32bit: return "float32";
		default: return NULL;
	}
}

ImageBitDepth ImagerUtils::StringToBitDepth(const char* depthString)
{
	if (strcmp(depthString, "unsigned8") == 0)
	{
		return ImageBitDepth::unsigned8bit;
	}
	else if (strcmp(depthString, "unsigned12in16") == 0)
	{
		return ImageBitDepth::unsigned12in16bit;
	}
	else if (strcmp(depthString, "unsigned16") == 0)
	{
		return ImageBitDepth::unsigned16bit;
	}
	else if (strcmp(depthString, "float32") == 0)
	{
		return ImageBitDepth::float32bit;
	}
	else
	{
		return ImageBitDepth::unsigned8bit;
	}
}

ImagerUtils::ImagerUtils() { }
