// AnalogServer.h: Interface description for Vrpn.AnalogServer
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

#pragma once

#include "vrpn_Analog.h"

#include "AnalogServerChannel.h"
#include "BaseTypes.h"
#include "Connection.h"

namespace Vrpn {
	public ref class AnalogServer: public Vrpn::IVrpnObject
	{
	public:
		AnalogServer(System::String ^name, Vrpn::Connection ^connection);
		AnalogServer(System::String ^name, Vrpn::Connection ^connection, System::Int32 numChannels);
		~AnalogServer();
		!AnalogServer();

		virtual void Update(); // from IVrpnObject
		virtual Connection^ GetConnection(); // from IVrpnObject
		property System::Boolean MuteWarnings // from IVrpnObject
		{
			virtual void set(System::Boolean);
			virtual System::Boolean get();
		}

		property cli::array<AnalogServerChannel ^> ^Channels
		{
			cli::array<AnalogServerChannel ^> ^get();
		}

		void Report();
		void Report(ServiceClass classOfService, System::DateTime time);

		void ReportChanges();
		void ReportChanges(ServiceClass classOfService, System::DateTime time);

		ref class AnalogChannelCollection
		{
		internal:
			AnalogChannelCollection(AnalogServer ^parent, System::Int32 numChannels);

		public:
			property AnalogServerChannel^ default [System::Int32]
			{
				AnalogServerChannel^ get(System::Int32 index);
				void set(System::Int32 index, AnalogServerChannel^ value);
			}
			property System::Int32 Length
			{
				System::Int32 get();
			}
		private:
			cli::array<AnalogServerChannel^> ^m_array;
			AnalogServer ^m_parent;
		};

		property AnalogChannelCollection ^AnalogChannels
		{
			AnalogChannelCollection ^get();
		};

		literal System::Int32 MaxChannels = vrpn_CHANNEL_MAX;	

	private:
		void Initialize(System::String ^name, Vrpn::Connection ^connection, System::Int32 numChannels);
		void UpdateChannels();

		::vrpn_Analog_Server *m_server;
		AnalogChannelCollection ^m_channels;
		System::Boolean m_disposed;
	};
}