// Connection.h: Interface description for Vrpn.Connection
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

#pragma once

#include "vrpn_Connection.h"

#include "BaseTypes.h"

namespace Vrpn {
	public ref class Connection
	{
	public:
		static Vrpn::Connection^ GetConnectionByName(
			System::String ^name);

		static Vrpn::Connection^ GetConnectionByName(
			System::String ^name,
			System::String ^localInLogfileName,
			System::String ^localOutLogfileName,
			System::String ^remoteInLogfileName,
			System::String ^remoteOutLogfileName,
			System::String ^nicIpAddress,
			System::Boolean forceReopen);

		static Vrpn::Connection^ FromPointer(
			::vrpn_Connection *pointer);
		
		~Connection();

		void Update();

		::vrpn_Connection* ToPointer();

		property System::Boolean DoingOkay
		{
			System::Boolean get();
		}

		property System::Boolean Connected
		{
			System::Boolean get();
		}

	private:
		Connection(::vrpn_Connection *connection);

		::vrpn_Connection *m_connection;
	};
}