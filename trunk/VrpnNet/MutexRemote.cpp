// MutexRemote.cpp: Implementation for Vrpn.MutexRemote
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

namespace Vrpn {
	namespace Internal {
		typedef void (*MutexCallback)(int);
		delegate void MutexEventHandler(int event);
	}
}

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;
using namespace Vrpn::Internal;

MutexRemote::MutexRemote(System::String ^name)
{
	Initialize(name, 0);
}

MutexRemote::MutexRemote(System::String ^name, Vrpn::Connection ^connection)
{
	Initialize(name, connection->ToPointer());
}

void MutexRemote::Initialize(System::String ^name, vrpn_Connection *lpConn)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);

	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());
	m_mutex = new Vrpn::Internal::MutexRemoteNative(ansiName, lpConn);

	Marshal::FreeHGlobal(hAnsiName);

	MutexEventHandler ^callback = gcnew MutexEventHandler(this, &MutexRemote::onVrpnEvent);
	gc_vrpnCallback = GCHandle::Alloc(callback);
	IntPtr pVrpnCallback = Marshal::GetFunctionPointerForDelegate(callback);
	m_mutex->setCallback(static_cast<MutexDelegate>(pVrpnCallback.ToPointer()));

	m_disposed = false;
}

MutexRemote::!MutexRemote()
{
	delete m_mutex;

	gc_vrpnCallback.Free();
	m_disposed = true;
}

MutexRemote::~MutexRemote()
{
	this->!MutexRemote();
}

void MutexRemote::Update()
{
	CHECK_DISPOSAL_STATUS();
	m_mutex->mainloop();
}

void MutexRemote::MuteWarnings::set(Boolean)
{
	throw gcnew NotSupportedException("vrpn_Mutex_Remote does not expose the shutup property.");
}

Boolean MutexRemote::MuteWarnings::get()
{
	throw gcnew NotSupportedException("vrpn_Mutex_Remote does not expose the shutup property.");
}

Connection^ MutexRemote::GetConnection()
{
	CHECK_DISPOSAL_STATUS();
	return Connection::FromPointer(m_mutex->connectionPtr());
}

Boolean MutexRemote::Available::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_mutex->isAvailable() != 0;
}

Boolean MutexRemote::HeldLocally::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_mutex->isHeldLocally() != 0;
}

Boolean MutexRemote::HeldRemotely::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_mutex->isHeldRemotely() != 0;
}

void MutexRemote::Request()
{
	CHECK_DISPOSAL_STATUS();
	m_mutex->request();
}

void MutexRemote::Release()
{
	CHECK_DISPOSAL_STATUS();
	m_mutex->release();
}

void MutexRemote::onVrpnEvent(int event)
{
	switch (event)
	{
	case Vrpn::Internal::RequestGranted:
		RequestGranted(this, EventArgs::Empty);
		break;

	case Vrpn::Internal::RequestDenied:
		RequestDenied(this, EventArgs::Empty);
		break;

	case Vrpn::Internal::Taken:
		Taken(this, EventArgs::Empty);
		break;

	case Vrpn::Internal::Released:
		Released(this, EventArgs::Empty);
		break;
	}
}