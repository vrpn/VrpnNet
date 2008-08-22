#include "stdafx.h"

#include "vrpn_Analog.h"

#include "BaseTypes.h"
#include "Connection.h"

namespace Vrpn {
	public ref class AnalogChangeEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property cli::array<System::Double> ^Channels;
	};

	public delegate void AnalogChangeEventHandler(System::Object ^sender,
		AnalogChangeEventArgs ^e);

	public ref class AnalogRemote: public Vrpn::IVrpnObject
	{
	public:
		AnalogRemote(System::String ^name);
		AnalogRemote(System::String ^name, Vrpn::Connection ^connection);
		~AnalogRemote();

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

		void Initialize(System::String ^name, vrpn_Connection *lpConn);
		void onAnalogChange(void *userData, const vrpn_ANALOGCB info);

		System::Runtime::InteropServices::GCHandle gc_callback;
	};
}