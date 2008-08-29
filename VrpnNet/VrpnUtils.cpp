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

void VrpnUtils::CreateTimeval(System::DateTime time, struct timeval *tm)
{
	DateTime epoch = DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind::Utc);

	TimeSpan span = time.ToUniversalTime().Subtract(epoch);

	__int64 ticks = span.Ticks;
	ticks = ticks % 10000000; // Strip seconds from 100ns ticks

	tm->tv_sec = static_cast<long>(span.TotalSeconds);	
	tm->tv_usec = static_cast<long>(ticks / 10);
}

void VrpnUtils::CreateQuat(Vrpn::Quaternion quat, double qArray[])
{
	qArray[Q_W] = quat.W;
	qArray[Q_X] = quat.X;
	qArray[Q_Y] = quat.Y;
	qArray[Q_Z] = quat.Z;
}

void VrpnUtils::CreateVector(Vrpn::Vector3 vec, double vArray[])
{
	vArray[0] = vec.X;
	vArray[1] = vec.Y;
	vArray[2] = vec.Z;
}

void VrpnUtils::CreateVector(Vrpn::Vector3 vec, float vArray[])
{
	vArray[0] = static_cast<float>(vec.X);
	vArray[1] = static_cast<float>(vec.Y);
	vArray[2] = static_cast<float>(vec.Z);
}
