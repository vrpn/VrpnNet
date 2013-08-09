// AnalogServer.cpp: Implementation for Vrpn.AnalogServer
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
#include "AnalogServer.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;

AnalogServer::AnalogChannelCollection::AnalogChannelCollection(Vrpn::AnalogServer ^parent, int numChannels)
{
	m_parent = parent;
	m_array = gcnew cli::array<AnalogServerChannel^>(numChannels);

	//Create all the channels (the constructor will put them into a known state)
	for (int i = 0; i < numChannels; i++)
	{
		m_array[i] = gcnew AnalogServerChannel();
	}
}

AnalogServerChannel^ AnalogServer::AnalogChannelCollection::default::get(int index)
{
	return m_array[index];
}

void AnalogServer::AnalogChannelCollection::default::set(int index, AnalogServerChannel^ value)
{
	if (m_parent->m_disposed)
		throw gcnew ObjectDisposedException("VRPN Object");
	m_array[index] = value;
	//m_parent->ReportChanges();  //I am not sure if these should be reported automatically
}

int AnalogServer::AnalogChannelCollection::Length::get()
{
	return m_array->Length;
}

AnalogServer::AnalogServer(System::String ^name, Vrpn::Connection ^connection)
{
	Initialize(name, connection, MaxChannels);
}

AnalogServer::AnalogServer(System::String ^name, Vrpn::Connection ^connection, int numChannels)
{
	Initialize(name, connection, numChannels);
}

void AnalogServer::Initialize(System::String ^name, Vrpn::Connection ^connection, int numChannels)
{
	if (numChannels > MaxChannels || numChannels < 1)
		throw gcnew ArgumentOutOfRangeException("Number of channels must be less than or equal to AnalogServer.MaxChannels.");

	IntPtr hName = Marshal::StringToHGlobalAnsi(name);
	const char *cName = static_cast<const char *>(hName.ToPointer());

	m_server = new ::vrpn_Analog_Server(cName, connection->ToPointer(), numChannels);
	Marshal::FreeHGlobal(hName);

	m_channels = gcnew AnalogChannelCollection(this, numChannels);
	Report(); //Provide the initialized values to the client

	m_disposed = false;
}

AnalogServer::!AnalogServer()
{
	delete m_server;
	m_channels = nullptr;
	m_disposed = true;
}

AnalogServer::~AnalogServer()
{
	this->!AnalogServer();
}

void AnalogServer::UpdateChannels()
{
	vrpn_float64 *serverChannels = m_server->channels();

	for (int i = 0; i < m_channels->Length; i++)
	{
		if (m_channels[i]->Dirty)
		{
			serverChannels[i] = m_channels[i]->Value;
			m_channels[i]->Dirty = false;
		}
	}
}

void AnalogServer::Update()
{
	CHECK_DISPOSAL_STATUS();
	m_server->mainloop();
}

Connection^ AnalogServer::GetConnection()
{
	CHECK_DISPOSAL_STATUS();
	return Connection::FromPointer(m_server->connectionPtr());
}

void AnalogServer::MuteWarnings::set(bool shutUp)
{
	CHECK_DISPOSAL_STATUS();
	m_server->shutup = shutUp;
}

bool AnalogServer::MuteWarnings::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_server->shutup;
}

void AnalogServer::Report()
{
	CHECK_DISPOSAL_STATUS();
	Report(ServiceClass::LowLatency, DateTime::Now);
}

void AnalogServer::ReportChanges()
{
	CHECK_DISPOSAL_STATUS();
	Report(ServiceClass::LowLatency, DateTime::Now);
}

void AnalogServer::Report(Vrpn::ServiceClass classOfService, System::DateTime time)
{
	CHECK_DISPOSAL_STATUS();
	timeval tm;
	VrpnUtils::CreateTimeval(time, &tm);

	UpdateChannels();
	m_server->report(safe_cast<vrpn_uint32>(classOfService), tm);
}

void AnalogServer::ReportChanges(Vrpn::ServiceClass classOfService, System::DateTime time)
{
	CHECK_DISPOSAL_STATUS();
	timeval tm;
	VrpnUtils::CreateTimeval(time, &tm);

	UpdateChannels();
	m_server->report_changes(safe_cast<vrpn_uint32>(classOfService), tm);
}

AnalogServer::AnalogChannelCollection ^AnalogServer::AnalogChannels::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_channels;
}
cli::array<AnalogServerChannel ^> ^AnalogServer::Channels::get()
{
	return m_channels;
}