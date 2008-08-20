#include "stdafx.h"
#include "ButtonRemote.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;

namespace {
	delegate void ButtonChangeCallback(void *userData, const vrpn_BUTTONCB info);
}

ButtonRemote::ButtonRemote(String ^name)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);

	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());
	m_button = new ::vrpn_Button_Remote(ansiName);

	Marshal::FreeHGlobal(hAnsiName);

	RegisterHandler();
}

ButtonRemote::ButtonRemote(System::String ^name, Vrpn::Connection ^connection)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);

	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());
	m_button = new ::vrpn_Button_Remote(ansiName, connection->ToPointer());

	Marshal::FreeHGlobal(hAnsiName);

	RegisterHandler();
}

ButtonRemote::~ButtonRemote()
{
	delete m_button;

	gc_callback.Free();
}

void ButtonRemote::Update()
{
	m_button->mainloop();
}

void ButtonRemote::MuteWarnings::set(Boolean shutUp)
{
	m_button->shutup = shutUp;
}

Boolean ButtonRemote::MuteWarnings::get()
{
	return m_button->shutup;
}

Connection^ ButtonRemote::GetConnection()
{
	return Connection::FromPointer(m_button->connectionPtr());
}

void ButtonRemote::RegisterHandler()
{
	ButtonChangeCallback ^btncb = gcnew ButtonChangeCallback(this, &ButtonRemote::onButtonChange);
	gc_callback = GCHandle::Alloc(btncb);
	IntPtr pBtnChange = Marshal::GetFunctionPointerForDelegate(btncb);
	vrpn_BUTTONCHANGEHANDLER pChangeHandler =
		static_cast<vrpn_BUTTONCHANGEHANDLER>(pBtnChange.ToPointer());
	m_button->register_change_handler(0, pChangeHandler);
}

void ButtonRemote::onButtonChange(void *userData, vrpn_BUTTONCB info)
{
	ButtonChangeEventArgs ^e = gcnew ButtonChangeEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Button = info.button;
	e->IsPressed = info.state != 0;

	ButtonChanged(this, e);
}