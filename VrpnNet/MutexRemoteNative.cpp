// MutexRemoteNative.cpp: Implementation for Vrpn.MutexRemoteNative
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
#include "MutexRemote.h"
#include "MutexRemoteNative.h"

using namespace Vrpn::Internal;

MutexRemoteNative::MutexRemoteNative(const char *name, vrpn_Connection *lpConn)
: vrpn_Mutex_Remote(name, lpConn)
{
	m_callback = 0;

	addRequestGrantedCallback(this, vrpnRequestGranted);
	addRequestDeniedCallback(this, vrpnRequestDenied);
	addTakeCallback(this, vrpnTaken);
	addReleaseCallback(this, vrpnReleased);
}

MutexRemoteNative::~MutexRemoteNative()
{
}

vrpn_Connection *MutexRemoteNative::connectionPtr() const
{
	return d_connection;
}

void MutexRemoteNative::setCallback(MutexDelegate callback)
{
	m_callback = callback;
}

void MutexRemoteNative::signalEvent(MutexRemoteEvent reason)
{
	m_callback(reason);
}

int MutexRemoteNative::vrpnRequestGranted(void *me)
{
	static_cast<MutexRemoteNative *>(me)->signalEvent(RequestGranted);
	return 0;
}

int MutexRemoteNative::vrpnRequestDenied(void *me)
{
	static_cast<MutexRemoteNative *>(me)->signalEvent(RequestDenied);
	return 0;
}

int MutexRemoteNative::vrpnTaken(void *me)
{
	static_cast<MutexRemoteNative *>(me)->signalEvent(Taken);
	return 0;
}

int MutexRemoteNative::vrpnReleased(void *me)
{
	static_cast<MutexRemoteNative *>(me)->signalEvent(Released);
	return 0;
}