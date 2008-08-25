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