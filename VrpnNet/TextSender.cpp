// TextSender.cpp: Implementation for Vrpn.TextSender
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
#include "Text.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;

TextSender::TextSender(String ^name)
{
	Initialize(name, 0);
}

TextSender::TextSender(System::String ^name, Vrpn::Connection ^connection)
{
	Initialize(name, connection->ToPointer());
}

TextSender::~TextSender()
{
	delete m_sender;
}

void TextSender::Initialize(System::String ^name, vrpn_Connection *lpConn)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);

	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());
	m_sender = new ::vrpn_Text_Sender(ansiName, lpConn);

	Marshal::FreeHGlobal(hAnsiName);
}

void TextSender::Update()
{
	m_sender->mainloop();
}

void TextSender::MuteWarnings::set(Boolean shutUp)
{
	m_sender->shutup = shutUp;
}

Boolean TextSender::MuteWarnings::get()
{
	return m_sender->shutup;
}

Connection^ TextSender::GetConnection()
{
	return Connection::FromPointer(m_sender->connectionPtr());
}

Int32 TextSender::SendMessage(System::String ^message)
{
	return SendMessage(message, TextSeverity::Normal, 0, DateTime::Now);
}

Int32 TextSender::SendMessage(System::String ^message, Vrpn::TextSeverity type)
{
	return SendMessage(message, type, 0, DateTime::Now);
}

Int32 TextSender::SendMessage(System::String ^message, Vrpn::TextSeverity type, unsigned int level)
{
	return SendMessage(message, type, level, DateTime::Now);
}

Int32 TextSender::SendMessage(System::String ^message, Vrpn::TextSeverity type, unsigned int level, System::DateTime time)
{
	IntPtr hAnsiMessage = Marshal::StringToHGlobalAnsi(message);

	const char *ansiMessage = static_cast<const char *>(hAnsiMessage.ToPointer());
	vrpn_TEXT_SEVERITY severity = static_cast<vrpn_TEXT_SEVERITY>(
		static_cast<unsigned int>(type));
	struct timeval unixTime = {0, 0};
	VrpnUtils::CreateTimeval(time, &unixTime);
	int result = m_sender->send_message(ansiMessage, severity, level, unixTime);
	
	Marshal::FreeHGlobal(hAnsiMessage);
	return result;
}