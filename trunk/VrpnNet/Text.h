// Text.h: Interface descriptions for Vrpn.TextSender and Vrpn.TextReceiver
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

#include "vrpn_Text.h"

namespace Vrpn {
	public enum class TextSeverity
	{
		Normal = 0,
		Warning = 1,
		Error = 2
	};

	public ref class TextReceivedEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property System::String^ String;
		property Vrpn::TextSeverity Type;
		property System::UInt32 Level;
	};

	public delegate void TextReceivedEventHandler(System::Object ^sender,
		Vrpn::TextReceivedEventArgs ^e);

	public ref class TextSender: public IVrpnObject
	{
	public:
		TextSender(System::String ^name);
		TextSender(System::String ^name, Vrpn::Connection ^connection);
		~TextSender();

		virtual void Update(); // from IVrpnObject
		virtual Connection^ GetConnection(); // from IVrpnObject
		property System::Boolean MuteWarnings // from IVrpnObject
		{
			virtual void set(System::Boolean);
			virtual System::Boolean get();
		}

		System::Int32 SendMessage(System::String ^message,
			Vrpn::TextSeverity type,
			System::UInt32 level,
			System::DateTime time);

		System::Int32 SendMessage(System::String ^message,
			Vrpn::TextSeverity type,
			System::UInt32 level);

		System::Int32 SendMessage(System::String ^message,
			Vrpn::TextSeverity type);

		System::Int32 SendMessage(System::String ^message);

	private:
		::vrpn_Text_Sender *m_sender;

		void Initialize(System::String ^name, vrpn_Connection *lpConn);
	};

	public ref class TextReceiver: public IVrpnObject
	{
	public:
		TextReceiver(System::String ^name);
		TextReceiver(System::String ^name, Vrpn::Connection ^connection);
		~TextReceiver();

		virtual void Update(); // from IVrpnObject
		virtual Connection^ GetConnection(); // from IVrpnObject
		property System::Boolean MuteWarnings // from IVrpnObject
		{
			virtual void set(System::Boolean);
			virtual System::Boolean get();
		}

		event TextReceivedEventHandler^ TextReceived;

	private:
		::vrpn_Text_Receiver *m_receiver;
		System::Runtime::InteropServices::GCHandle gc_callback;

		void onTextReceived(void *userData, const vrpn_TEXTCB info);
		void Initialize(System::String ^name, vrpn_Connection *lpConn);
	};
}