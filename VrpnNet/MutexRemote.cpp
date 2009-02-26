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

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;
using namespace Vrpn::Internal;

namespace {
	typedef void (*MutexCallback)(int);
	delegate void MutexEventHandler(int event);
}

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
	/*
	throw gcnew NotImplementedException(
		"Mutex event handlers are currently incompatible with managed code.");
	*/

	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);

	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());
	m_mutex = new Vrpn::Internal::MutexRemoteNative(ansiName, lpConn);

	Marshal::FreeHGlobal(hAnsiName);

	MutexEventHandler ^callback = gcnew MutexEventHandler(this, &MutexRemote::onVrpnEvent);
	gc_vrpnCallback = GCHandle::Alloc(callback);
	IntPtr pVrpnCallback = Marshal::GetFunctionPointerForDelegate(callback);
	m_mutex->setCallback(static_cast<MutexDelegate>(pVrpnCallback.ToPointer()));
}

MutexRemote::~MutexRemote()
{
	delete m_mutex;

	gc_vrpnCallback.Free();
}

void MutexRemote::Update()
{
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
	return Connection::FromPointer(m_mutex->connectionPtr());
}

Boolean MutexRemote::Available::get()
{
	return m_mutex->isAvailable() != 0;
}

Boolean MutexRemote::HeldLocally::get()
{
	return m_mutex->isHeldLocally() != 0;
}

Boolean MutexRemote::HeldRemotely::get()
{
	return m_mutex->isHeldRemotely() != 0;
}

void MutexRemote::Request()
{
	m_mutex->request();
}

void MutexRemote::Release()
{
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