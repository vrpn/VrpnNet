// AnalogRemote.cpp: Implementation for Vrpn.AnalogRemote
//
// Copyright (c) 2008 Chris VanderKnyff
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
#include "AnalogRemote.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;

namespace {
	delegate void AnalogChangeCallback(void *userData, const vrpn_ANALOGCB info);
}

AnalogRemote::AnalogRemote(String ^name)
{
	Initialize(name, 0);
}

AnalogRemote::AnalogRemote(System::String ^name, Vrpn::Connection ^connection)
{
	Initialize(name, connection->ToPointer());
}

void AnalogRemote::Initialize(System::String ^name, vrpn_Connection *lpConn)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);

	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());
	m_analog = new ::vrpn_Analog_Remote(ansiName, lpConn);

	Marshal::FreeHGlobal(hAnsiName);

	AnalogChangeCallback ^callback = gcnew AnalogChangeCallback(this, &AnalogRemote::onAnalogChange);
	gc_callback = GCHandle::Alloc(callback);
	IntPtr pAnalogChange = Marshal::GetFunctionPointerForDelegate(callback);
	vrpn_ANALOGCHANGEHANDLER pCallbackFunc = 
		static_cast<vrpn_ANALOGCHANGEHANDLER>(pAnalogChange.ToPointer());

	m_analog->register_change_handler(0, pCallbackFunc);
}

AnalogRemote::~AnalogRemote()
{
	delete m_analog;

	gc_callback.Free();
}

void AnalogRemote::Update()
{
	m_analog->mainloop();
}

void AnalogRemote::MuteWarnings::set(Boolean shutUp)
{
	m_analog->shutup = shutUp;
}

Boolean AnalogRemote::MuteWarnings::get()
{
	return m_analog->shutup;
}

Connection^ AnalogRemote::GetConnection()
{
	return Connection::FromPointer(m_analog->connectionPtr());
}

void AnalogRemote::onAnalogChange(void *, const vrpn_ANALOGCB info)
{
	AnalogChangeEventArgs ^e = gcnew AnalogChangeEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Channels = gcnew array<double>(info.num_channel);

	pin_ptr<double> clrChannels = &e->Channels[0];

	::memcpy_s(clrChannels, info.num_channel * sizeof(double), 
		info.channel, vrpn_CHANNEL_MAX * sizeof(double));

	AnalogChanged(this, e);
}