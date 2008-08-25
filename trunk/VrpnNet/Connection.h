#pragma once

#include "vrpn_Connection.h"

#include "BaseTypes.h"

namespace Vrpn {
	public ref class Connection
	{
	public:
		static Vrpn::Connection^ GetConnectionByName(
			System::String ^name);

		/*
		static Vrpn::Connection^ GetConnectionByName(
			System::String ^name,
			System::String ^localInLogfileName,
			System::String ^localOutLogfileName,
			System::String ^remoteInLogfileName,
			System::String ^remoteOutLogfileName,
			System::String ^nicIpAddress,
			System::Boolean ^forceReopen);
		*/

		static Vrpn::Connection^ FromPointer(
			vrpn_Connection *pointer);
		
		~Connection();

		void Update();

		vrpn_Connection* ToPointer();

		property System::Boolean DoingOkay
		{
			System::Boolean get();
		}

		property System::Boolean Connected
		{
			System::Boolean get();
		}

	private:
		Connection(vrpn_Connection *connection);

		vrpn_Connection *m_connection;
	};
}