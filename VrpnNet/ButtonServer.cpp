// ButtonServer.cpp: Implementation for Vrpn.ButtonServer
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
#include "ButtonServer.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;

ButtonServer::ButtonCollection::ButtonCollection(Vrpn::ButtonServer ^parent, int numButtons)
{
	m_parent = parent;
	m_array = gcnew cli::array<bool>(numButtons);

	// Put the collection in a known state
	for (int i = 0; i < numButtons; i++)
	{
		m_parent->m_server->set_button(i, 0);
		m_array[i] = false;
	}
}

bool ButtonServer::ButtonCollection::default::get(int index)
{
	return m_array[index];
}

void ButtonServer::ButtonCollection::default::set(int index, bool state)
{
	if (m_parent->m_disposed)
		throw gcnew ObjectDisposedException("VRPN Object");
	m_parent->m_server->set_button(index, state ? 1 : 0);
	m_array[index] = state;
}

ButtonServer::ButtonServer(String ^name, Connection ^connection)
{
	Initialize(name, connection, 1);
}

ButtonServer::ButtonServer(String ^name, Connection ^connection, int numButtons)
{
	Initialize(name, connection, numButtons);
}

void ButtonServer::Initialize(System::String ^name, Vrpn::Connection ^connection, int numButtons)
{
	if (numButtons < 0 || numButtons > MaxButtons)
		throw gcnew ArgumentOutOfRangeException("Button count must be nonnegative and not greater than ButtonServer.MaxButtons.");

	IntPtr hName = Marshal::StringToHGlobalAnsi(name);
	const char *cName = static_cast<const char *>(hName.ToPointer());

	m_server = new vrpn_Button_Server(cName, connection->ToPointer(), numButtons);
	Marshal::FreeHGlobal(hName);

	m_buttons = gcnew ButtonCollection(this, numButtons);
	m_disposed = false;
}

ButtonServer::!ButtonServer()
{
	delete m_server;
	m_buttons = nullptr;
}

ButtonServer::~ButtonServer()
{
	this->!ButtonServer();
}

Connection ^ButtonServer::GetConnection()
{
	CHECK_DISPOSAL_STATUS();
	return Connection::FromPointer(m_server->connectionPtr());
}

bool ButtonServer::MuteWarnings::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_server->shutup;
}

void ButtonServer::MuteWarnings::set(bool shutUp)
{
	CHECK_DISPOSAL_STATUS();
	m_server->shutup = shutUp;
}

void ButtonServer::Update()
{
	CHECK_DISPOSAL_STATUS();
	m_server->mainloop();
}

ButtonServer::ButtonCollection ^ButtonServer::Buttons::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_buttons;
}