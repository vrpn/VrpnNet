// ButtonServer.h: Interface description for Vrpn.ButtonServer
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

#include "BaseTypes.h"
#include "Connection.h"
#include "vrpn_Button.h"

namespace Vrpn {
	public ref class ButtonServer: public Vrpn::IVrpnObject
	{
	public:
		ButtonServer(System::String ^name, Vrpn::Connection ^connection);
		ButtonServer(System::String ^name, Vrpn::Connection ^connection, System::Int32 numButtons);
		~ButtonServer();

		virtual void Update(); // from IVrpnObject
		virtual Vrpn::Connection ^GetConnection(); // from IVrpnObject
		property System::Boolean MuteWarnings // from IVrpnObject
		{
			virtual void set(System::Boolean shutUp);
			virtual System::Boolean get();
		}

		ref class ButtonCollection
		{
		internal:
			ButtonCollection(ButtonServer ^parent, System::Int32 numButtons);
		
		public:
			property System::Boolean default [System::Int32]
			{
				System::Boolean get(System::Int32 index);
				void set(System::Int32 index, System::Boolean value);
			}

		private:
			cli::array<System::Boolean> ^m_array;
			ButtonServer ^m_parent;
		};

		property ButtonCollection ^Buttons
		{
			ButtonCollection ^get();
		};

		literal int MaxButtons = vrpn_BUTTON_MAX_BUTTONS;

	private:
		::vrpn_Button_Server *m_server;
		ButtonCollection ^m_buttons;

		void Initialize(System::String ^name, Vrpn::Connection ^connection, System::Int32 numButtons);
	};
}