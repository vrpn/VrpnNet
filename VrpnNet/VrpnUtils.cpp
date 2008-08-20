#include "stdafx.h"

#include "BaseTypes.h"

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace Vrpn;

VrpnUtils::VrpnUtils() { }

DateTime VrpnUtils::ConvertTimeval(struct timeval tm)
{
	long long ticks; // 100-nanosecond increments

	// Load the Unix epoch
	ticks = (gcnew DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind::Utc))->Ticks;

	ticks += tm.tv_usec * 10; // microseconds to 100-nanoseconds
	ticks += tm.tv_sec * 10000000; // seconds to 100-nanoseconds

	return DateTime(ticks, DateTimeKind::Utc);
}