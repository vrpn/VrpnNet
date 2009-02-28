// AnalogRemote.h: Interface description for Vrpn.AnalogRemote
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

#include "vrpn_Analog.h"

#include "BaseTypes.h"
#include "Connection.h"

namespace Vrpn {
	public ref class AnalogChangeEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property cli::array<double> ^Channels;
	};

	public delegate void AnalogChangeEventHandler(System::Object ^sender,
		AnalogChangeEventArgs ^e);

	public ref class AnalogRemote: public Vrpn::IVrpnObject
	{
	public:
		AnalogRemote(System::String ^name);
		AnalogRemote(System::String ^name, Vrpn::Connection ^connection);
		~AnalogRemote();
		!AnalogRemote();

		virtual void Update(); // from IVrpnObject
		virtual Connection^ GetConnection(); // from IVrpnObject
		property System::Boolean MuteWarnings // from IVrpnObject
		{
			virtual void set(System::Boolean);
			virtual System::Boolean get();
		}

		event AnalogChangeEventHandler^ AnalogChanged;

	private:
		::vrpn_Analog_Remote *m_analog;
		System::Boolean m_disposed;

		void Initialize(System::String ^name, vrpn_Connection *lpConn);
		void onAnalogChange(void *userData, const vrpn_ANALOGCB info);

		System::Runtime::InteropServices::GCHandle gc_callback;
	};
}