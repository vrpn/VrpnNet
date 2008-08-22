#include "stdafx.h"

#include "BaseTypes.h"
#include "quat.h"

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace Vrpn;

VrpnUtils::VrpnUtils() { }

DateTime VrpnUtils::ConvertTimeval(struct timeval tm)
{
	DateTime epoch = DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind::Utc);

	return epoch.AddSeconds(tm.tv_sec).AddTicks(tm.tv_usec * 10I64);
}

Vector3 VrpnUtils::ConvertVector(const double vec[3])
{
	return Vector3(vec[0], vec[1], vec[2]);
}

Quaternion VrpnUtils::ConvertQuat(const double quat[4])
{
	return Quaternion(quat[Q_X], quat[Q_Y], quat[Q_Z], quat[Q_W]);
}