// VrpnUtils.cpp: Various utility methods for C++/CLR interop
//
// Copyright (c) 2008-2009 Chris VanderKnyff
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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
