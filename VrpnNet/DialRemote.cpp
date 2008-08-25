#include "stdafx.h"
#include "DialRemote.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;

namespace {
	delegate void DialChangeCallback(void *userData, const vrpn_DIALCB info);
}

DialRemote::DialRemote(String ^name)
{
	Initialize(name, 0);
}

DialRemote::DialRemote(System::String ^name, Vrpn::Connection ^connection)
{
	Initialize(name, connection->ToPointer());
}

void DialRemote::Initialize(System::String ^name, vrpn_Connection *lpConn)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);

	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());
	m_dial = new ::vrpn_Dial_Remote(ansiName, lpConn);

	Marshal::FreeHGlobal(hAnsiName);

	DialChangeCallback ^callback = gcnew DialChangeCallback(this, &DialRemote::onDialChange);
	gc_callback = GCHandle::Alloc(callback);
	IntPtr pDialChange = Marshal::GetFunctionPointerForDelegate(callback);
	vrpn_DIALCHANGEHANDLER pCallbackFunc = 
		static_cast<vrpn_DIALCHANGEHANDLER>(pDialChange.ToPointer());

	m_dial->register_change_handler(0, pCallbackFunc);
}

DialRemote::~DialRemote()
{
	delete m_dial;

	gc_callback.Free();
}

void DialRemote::Update()
{
	m_dial->mainloop();
}

void DialRemote::MuteWarnings::set(Boolean shutUp)
{
	m_dial->shutup = shutUp;
}

Boolean DialRemote::MuteWarnings::get()
{
	return m_dial->shutup;
}

Connection^ DialRemote::GetConnection()
{
	return Connection::FromPointer(m_dial->connectionPtr());
}

void DialRemote::onDialChange(void *, const vrpn_DIALCB info)
{
	DialChangeEventArgs ^e = gcnew DialChangeEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->DialIndex = info.dial;
	e->Change = info.change;

	DialChanged(this, e);
}