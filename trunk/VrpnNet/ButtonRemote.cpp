// ButtonRemote.cpp: Implementation for Vrpn.ButtonRemote
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
#include "ButtonRemote.h"

namespace Vrpn {
	namespace Internal {
		delegate void ButtonChangeCallback(void *userData, const vrpn_BUTTONCB info);
	}
}

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;
using namespace Vrpn::Internal;

ButtonRemote::ButtonRemote(String ^name)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);

	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());
	m_button = new ::vrpn_Button_Remote(ansiName);

	Marshal::FreeHGlobal(hAnsiName);

	RegisterHandler();

	m_disposed = false;
}

ButtonRemote::ButtonRemote(System::String ^name, Vrpn::Connection ^connection)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);

	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());
	m_button = new ::vrpn_Button_Remote(ansiName, connection->ToPointer());

	Marshal::FreeHGlobal(hAnsiName);

	RegisterHandler();

	m_disposed = false;
}

ButtonRemote::!ButtonRemote()
{
	delete m_button;

	gc_callback.Free();
	m_disposed = true;
}

ButtonRemote::~ButtonRemote()
{
	this->!ButtonRemote();
}

void ButtonRemote::Update()
{
	CHECK_DISPOSAL_STATUS();
	m_button->mainloop();
}

void ButtonRemote::MuteWarnings::set(Boolean shutUp)
{
	CHECK_DISPOSAL_STATUS();
	m_button->shutup = shutUp;
}

Boolean ButtonRemote::MuteWarnings::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_button->shutup;
}

Connection^ ButtonRemote::GetConnection()
{
	CHECK_DISPOSAL_STATUS();
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

void ButtonRemote::onButtonChange(void *, const vrpn_BUTTONCB info)
{
	ButtonChangeEventArgs ^e = gcnew ButtonChangeEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Button = info.button;
	e->IsPressed = info.state != 0;

	ButtonChanged(this, e);
}