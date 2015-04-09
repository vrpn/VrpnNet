// Connection.cpp: Implementation for Vrpn.Connection
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
	m_disposed = false;
}

Connection::~Connection()
{
	this->!Connection();
}

Connection::!Connection()
{
	m_connection->removeReference();
	m_disposed = true;
}

vrpn_Connection* Connection::ToPointer()
{
	CHECK_DISPOSAL_STATUS();
	return m_connection;
}

void Connection::Update()
{
	CHECK_DISPOSAL_STATUS();
	m_connection->mainloop();
}

//This is to limit the bandwidth used for the Imager, otherwise you can ignore it
///<summary>
///Limits the number of messages parsed by the connection in a single Update() call.  Can be ignored if the imager is not in use.
///</summary>
///<param name="messageLimit">The max number of messages the connection should handle on a given call to Update().  A value of 0 will turn off the message limit.</param>
///<remarks>
///The name comes from title sequence of "The Jetsons."
///</remarks>
void Connection::JaneStopThisCrazyThing(System::UInt32 messageLimit)
{
	CHECK_DISPOSAL_STATUS();
	m_connection->Jane_stop_this_crazy_thing(messageLimit);
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

	Marshal::FreeHGlobal(hName);

	if (!conn)
		throw gcnew VrpnException();

	return gcnew Connection(conn);
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

	// FreeHGlobal is defined to be a no-op on zero (null) pointers
	Marshal::FreeHGlobal(hName);
	Marshal::FreeHGlobal(hLocalLogIn);
	Marshal::FreeHGlobal(hLocalLogOut);
	Marshal::FreeHGlobal(hRemoteLogIn);
	Marshal::FreeHGlobal(hRemoteLogOut);
	Marshal::FreeHGlobal(hAddress);

	if (!cConn)
		throw gcnew VrpnException();

	return gcnew Connection(cConn);
}

Connection^ Connection::CreateServerConnection(System::String ^name)
{
	return CreateServerConnection(name, nullptr, nullptr);
}

Connection^ Connection::CreateServerConnection(System::String ^name, 
	System::String ^localInLogfileName, System::String ^localOutLogfileName)
{
	const char *cName, *cLocalLogIn, *cLocalLogOut;
	IntPtr hName, hLocalLogIn, hLocalLogOut;

	// Temporarily suppress "conditional expression is constant" warnings.
	// We know that we're testing while(false) in the loop. It is expected.
#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4127)
#endif

	VRPNNET_ALLOCSTRING(Name, name);
	VRPNNET_ALLOCSTRING(LocalLogIn, localInLogfileName);
	VRPNNET_ALLOCSTRING(LocalLogOut, localOutLogfileName);

	// Restore normal warnings
#ifdef _MSC_VER
# pragma warning(pop)
#endif

	::vrpn_Connection *cConn = ::vrpn_create_server_connection(cName, cLocalLogIn, cLocalLogOut);

	Marshal::FreeHGlobal(hName);
	Marshal::FreeHGlobal(hLocalLogIn);
	Marshal::FreeHGlobal(hLocalLogOut);

	if (!cConn)
		throw gcnew VrpnException();

	return gcnew Connection(cConn);
}

Connection^ Connection::CreateServerConnection()
{
	return CreateServerConnection(DefaultListenPort);
}

Connection^ Connection::CreateServerConnection(System::Int32 port)
{
	return CreateServerConnection(port, nullptr, nullptr, nullptr);
}

Connection^ Connection::CreateServerConnection(System::Int32 port, 
	System::String ^localInLogfileName, System::String ^localOutLogfileName, System::String ^nicName)
{
	const char *cLocalLogIn, *cLocalLogOut, *cNicName;
	IntPtr hLocalLogIn, hLocalLogOut, hNicName;

	// Temporarily suppress "conditional expression is constant" warnings.
	// We know that we're testing while(false) in the loop. It is expected.
#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4127)
#endif

	VRPNNET_ALLOCSTRING(LocalLogIn, localInLogfileName);
	VRPNNET_ALLOCSTRING(LocalLogOut, localOutLogfileName);
	VRPNNET_ALLOCSTRING(NicName, nicName);

	// Restore normal warnings
#ifdef _MSC_VER
# pragma warning(pop)
#endif

	::vrpn_Connection *cConn = ::vrpn_create_server_connection(port, cLocalLogIn, 
		cLocalLogOut, cNicName);

	Marshal::FreeHGlobal(hLocalLogIn);
	Marshal::FreeHGlobal(hLocalLogOut);
	Marshal::FreeHGlobal(hNicName);

	if (!cConn)
		throw gcnew VrpnException();

	return gcnew Connection(cConn);
}

Boolean Connection::DoingOkay::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_connection->doing_okay() != 0;
}

Boolean Connection::Connected::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_connection->connected() != 0;
}