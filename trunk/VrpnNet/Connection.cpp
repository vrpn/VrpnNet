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