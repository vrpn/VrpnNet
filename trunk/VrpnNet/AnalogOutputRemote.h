// AnalogOutputRemote.h: Interface description for Vrpn.AnalogOutputRemote
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

#include "vrpn_Analog_Output.h"

#include "BaseTypes.h"
#include "Connection.h"

namespace Vrpn {
	public ref class AnalogOutputRemote: public IVrpnObject {
	public:
		AnalogOutputRemote(System::String ^name);
		AnalogOutputRemote(System::String ^name, Vrpn::Connection ^connection);
		~AnalogOutputRemote();

		virtual void Update(); // from IVrpnObject
		virtual Connection^ GetConnection(); // from IVrpnObject
		property System::Boolean MuteWarnings // from IVrpnObject
		{
			virtual void set(System::Boolean);
			virtual System::Boolean get();
		}

		System::Boolean RequestChannelChange(System::Int64 channel,
			double value);

		System::Boolean RequestChannelChange(System::Int64 channel,
			double value, ServiceClass sc);

		System::Boolean RequestChannelChange(
			cli::array<double> ^channels);

		System::Boolean RequestChannelChange(
			cli::array<double> ^channels, ServiceClass sc);
	
	private:
		::vrpn_Analog_Output_Remote *m_analogOut;
		
		void Initialize(System::String ^name, vrpn_Connection *lpConn);
	};
}