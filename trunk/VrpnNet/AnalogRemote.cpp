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

	/*
	// TODO: Investigate possibility of doing this with memcpy
	for (int i = 0; i < info.num_channel; i++)
	{
		e->Channels[i] = info.channel[i];
	}
	*/

	AnalogChanged(this, e);
}