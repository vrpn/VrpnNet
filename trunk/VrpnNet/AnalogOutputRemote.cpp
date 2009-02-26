// AnalogOutputRemote.cpp: Implementation for Vrpn.AnalogOutputRemote
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
#include "AnalogOutputRemote.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;

AnalogOutputRemote::AnalogOutputRemote(String ^name)
{
	Initialize(name, 0);
}

AnalogOutputRemote::AnalogOutputRemote(String ^name, Connection ^c)
{
	Initialize(name, c->ToPointer());
}

AnalogOutputRemote::~AnalogOutputRemote()
{
	delete m_analogOut;
}

void AnalogOutputRemote::Initialize(String ^name, vrpn_Connection *lpConn)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);

	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());
	m_analogOut = new ::vrpn_Analog_Output_Remote(ansiName, lpConn);

	Marshal::FreeHGlobal(hAnsiName);
}

void AnalogOutputRemote::Update()
{
	m_analogOut->mainloop();
}

void AnalogOutputRemote::MuteWarnings::set(Boolean shutUp)
{
	m_analogOut->shutup = shutUp;
}

Boolean AnalogOutputRemote::MuteWarnings::get()
{
	return m_analogOut->shutup;
}

Connection^ AnalogOutputRemote::GetConnection()
{
	return Connection::FromPointer(m_analogOut->connectionPtr());
}

Boolean AnalogOutputRemote::RequestChannelChange(Int64 channel, Double value)
{
	return RequestChannelChange(channel, value, ServiceClass::Reliable);
}

Boolean AnalogOutputRemote::RequestChannelChange(Int64 channel, Double value, ServiceClass sc)
{
	if (channel > 0xFFFFFFFFUL)
		throw gcnew ArgumentOutOfRangeException("channel", "Value must fit in a vrpn_uint32 type");

	return m_analogOut->request_change_channel_value(
		static_cast<unsigned int>(channel),
		value, static_cast<unsigned int>(sc));
}

Boolean AnalogOutputRemote::RequestChannelChange(array<Double> ^channels)
{
	return RequestChannelChange(channels, ServiceClass::Reliable);
}

Boolean AnalogOutputRemote::RequestChannelChange(array<Double> ^channels, ServiceClass sc)
{
	if (channels->LongLength > 0xFFFFFFFFUL)
		throw gcnew ArgumentException(
			"VRPN AnalogOutput class supports only 2^32-1 channels", "channels");

	pin_ptr<Double> pChannels = &channels[0];

	return m_analogOut->request_change_channels(channels->Length, pChannels, 
		static_cast<unsigned int>(sc));
}