// TextReceiver.cpp: Implementation for Vrpn.TextReceiver
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
#include "Text.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;

namespace {
	delegate void TextReceivedCallback(void *userData, const vrpn_TEXTCB info);
}

TextReceiver::TextReceiver(String ^name)
{
	Initialize(name, 0);
}

TextReceiver::TextReceiver(System::String ^name, Vrpn::Connection ^connection)
{
	Initialize(name, connection->ToPointer());
}

void TextReceiver::Initialize(System::String ^name, vrpn_Connection *lpConn)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);

	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());
	m_receiver = new ::vrpn_Text_Receiver(ansiName, lpConn);

	Marshal::FreeHGlobal(hAnsiName);

	TextReceivedCallback ^callback = gcnew TextReceivedCallback(this, &TextReceiver::onTextReceived);
	gc_callback = GCHandle::Alloc(callback);
	IntPtr pTextReceipt = Marshal::GetFunctionPointerForDelegate(callback);
	vrpn_TEXTHANDLER pCallbackFunc = 
		static_cast<vrpn_TEXTHANDLER>(pTextReceipt.ToPointer());

	m_receiver->register_message_handler(0, pCallbackFunc);
}

TextReceiver::~TextReceiver()
{
	delete m_receiver;

	gc_callback.Free();
}

void TextReceiver::Update()
{
	m_receiver->mainloop();
}

void TextReceiver::MuteWarnings::set(Boolean shutUp)
{
	m_receiver->shutup = shutUp;
}

Boolean TextReceiver::MuteWarnings::get()
{
	return m_receiver->shutup;
}

Connection^ TextReceiver::GetConnection()
{
	return Connection::FromPointer(m_receiver->connectionPtr());
}

void TextReceiver::onTextReceived(void *, const vrpn_TEXTCB info)
{
	TextReceivedEventArgs ^e = gcnew TextReceivedEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Level = info.level;
	e->Type = safe_cast<TextSeverity>(info.type);
	e->String = gcnew String(info.message);

	TextReceived(this, e);
}
