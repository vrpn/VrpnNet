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

#define VRPNNET_ALLOCSTRING(suffix, param) do { \
	if (param != nullptr) { \
		h ## suffix = Marshal::StringToHGlobalAnsi(param); \
		c ## suffix = static_cast<const char *>(h##suffix.ToPointer()); \
	} \
	else { \
		h ## suffix = IntPtr::Zero; \
		c ## suffix = 0; \
	} \
	} while (false);

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
	if (!conn)
		throw gcnew VrpnException();

	Connection ^retVal = gcnew Connection(conn);

	Marshal::FreeHGlobal(hName);

	return retVal;
}

Connection^ Connection::GetConnectionByName(System::String ^name,
	System::String ^localInLogfileName, System::String ^localOutLogfileName,
	System::String ^remoteInLogfileName, System::String ^remoteOutLogfileName,
	System::String ^nicIpAddress, System::Boolean forceReopen)
{
	const char *cName, *cLocalLogIn, *cLocalLogOut;
	const char *cRemoteLogIn, *cRemoteLogOut, *cAddress;

	IntPtr hName, hLocalLogIn, hLocalLogOut, hRemoteLogIn, hRemoteLogOut, hAddress;

	// Temporarily suppress "conditional expression is constant" warnings.
	// We know that we're testing while(false) in the loop. It is expected.
#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4127)
#endif

	VRPNNET_ALLOCSTRING(Name, name);
	VRPNNET_ALLOCSTRING(LocalLogIn, localInLogfileName);
	VRPNNET_ALLOCSTRING(LocalLogOut, localOutLogfileName);
	VRPNNET_ALLOCSTRING(RemoteLogIn, remoteInLogfileName);
	VRPNNET_ALLOCSTRING(RemoteLogOut, remoteOutLogfileName);
	VRPNNET_ALLOCSTRING(Address, nicIpAddress);

	// Restore normal warnings
#ifdef _MSC_VER
# pragma warning(pop)
#endif

	vrpn_Connection *cConn = ::vrpn_get_connection_by_name(cName,
		cLocalLogIn, cLocalLogOut, cRemoteLogIn, cRemoteLogOut, 
		cAddress, forceReopen);
	if (!cConn)
		throw gcnew VrpnException();

	Connection ^retVal = gcnew Connection(cConn);

	// FreeHGlobal is defined to be a no-op on zero (null) pointers
	Marshal::FreeHGlobal(hName);
	Marshal::FreeHGlobal(hLocalLogIn);
	Marshal::FreeHGlobal(hLocalLogOut);
	Marshal::FreeHGlobal(hRemoteLogIn);
	Marshal::FreeHGlobal(hRemoteLogOut);
	Marshal::FreeHGlobal(hAddress);

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