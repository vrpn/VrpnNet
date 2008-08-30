// Connection.cpp: Implementation for Vrpn.Connection
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

#include "Connection.h"

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace Vrpn;

Connection::Connection(vrpn_Connection *connection)
{
	m_connection = connection;
}

Connection::~Connection()
{
	delete m_connection;
}

vrpn_Connection* Connection::ToPointer()
{
	return m_connection;
}

void Connection::Update()
{
	m_connection->mainloop();
}

Connection^ Connection::FromPointer(vrpn_Connection *pointer)
{
	return gcnew Connection(pointer);
}

Connection^ Connection::GetConnectionByName(System::String ^name)
{
	IntPtr hName = Marshal::StringToHGlobalAnsi(name);
	const char *cName = static_cast<const char *>(hName.ToPointer());

	vrpn_Connection *conn = ::vrpn_get_connection_by_name(cName);
	Connection^ retVal = gcnew Connection(conn);

	Marshal::FreeHGlobal(hName);

	return retVal;
}

Boolean Connection::DoingOkay::get()
{
	return m_connection->doing_okay() != 0;
}

Boolean Connection::Connected::get()
{
	return m_connection->connected() != 0;
}