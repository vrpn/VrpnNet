// ButtonRemote.h: Interface description for Vrpn.ButtonRemote
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

#include "vrpn_Button.h"

#include "BaseTypes.h"
#include "Connection.h"

namespace Vrpn {
	public ref class ButtonChangeEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property System::Int32 Button;
		property System::Boolean IsPressed;
	};

	public delegate void ButtonChangeEventHandler(System::Object ^sender,
		ButtonChangeEventArgs ^e);

	public ref class ButtonRemote: public Vrpn::IVrpnObject
	{
	public:
		ButtonRemote(System::String ^name);
		ButtonRemote(System::String ^name, Vrpn::Connection ^connection);
		~ButtonRemote();
		!ButtonRemote();

		virtual void Update(); // from IVrpnObject
		virtual Connection^ GetConnection(); // from IVrpnObject
		property System::Boolean MuteWarnings // from IVrpnObject
		{
			virtual void set(System::Boolean);
			virtual System::Boolean get();
		}

		event ButtonChangeEventHandler^ ButtonChanged;

	private:
		::vrpn_Button_Remote *m_button;

		void RegisterHandler();
		void onButtonChange(void *userData, const vrpn_BUTTONCB info);

		System::Runtime::InteropServices::GCHandle gc_callback;
		System::Boolean m_disposed;
	};
}