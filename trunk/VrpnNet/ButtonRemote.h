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
	};
}