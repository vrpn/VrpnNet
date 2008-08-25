#pragma once

#include "vrpn_Dial.h"
#include "BaseTypes.h"
#include "Connection.h"

namespace Vrpn {
	public ref class DialChangeEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property System::Int32 DialIndex;
		property System::Double Change;
	};

	public delegate void DialChangeEventHandler(System::Object ^sender,
		DialChangeEventArgs ^e);

	public ref class DialRemote: public Vrpn::IVrpnObject
	{
	public:
		DialRemote(System::String ^name);
		DialRemote(System::String ^name, Vrpn::Connection ^connection);
		~DialRemote();

		virtual void Update(); // from IVrpnObject
		virtual Connection^ GetConnection(); // from IVrpnObject
		property System::Boolean MuteWarnings // from IVrpnObject
		{
			virtual void set(System::Boolean);
			virtual System::Boolean get();
		}

		event DialChangeEventHandler^ DialChanged;

	private:
		::vrpn_Dial_Remote *m_dial;

		void Initialize(System::String ^name, vrpn_Connection *lpConn);
		void onDialChange(void *userData, const vrpn_DIALCB info);

		System::Runtime::InteropServices::GCHandle gc_callback;
	};
}