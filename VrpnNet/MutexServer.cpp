// MutexServer.cpp: Implementation for Vrpn.MutexServer
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
#include "MutexServer.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;

MutexServer::MutexServer(System::String ^name, Vrpn::Connection ^connection)
{
	IntPtr hName = Marshal::StringToHGlobalAnsi(name);
	const char *cName = static_cast<const char *>(hName.ToPointer());

	m_server = new ::vrpn_Mutex_Server(cName, connection->ToPointer());
	m_connection = connection;
	Marshal::FreeHGlobal(hName);

	m_disposed = false;
}

MutexServer::!MutexServer()
{
	delete m_server;
	m_server = 0;
}

MutexServer::~MutexServer()
{
	this->!MutexServer();
}

void MutexServer::Update()
{
	CHECK_DISPOSAL_STATUS();
	m_server->mainloop();
}

Connection ^MutexServer::GetConnection()
{
	CHECK_DISPOSAL_STATUS();
	return m_connection;
}

void MutexServer::MuteWarnings::set(bool)
{
	throw gcnew NotSupportedException("vrpn_Mutex_Server does not expose the shutup property.");
}

bool MutexServer::MuteWarnings::get()
{
	throw gcnew NotSupportedException("vrpn_Mutex_Server does not expose the shutup property.");
}
