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
			System::Double value);

		System::Boolean RequestChannelChange(System::Int64 channel,
			System::Double value, ServiceClass sc);

		System::Boolean RequestChannelChange(
			cli::array<System::Double> ^channels);

		System::Boolean RequestChannelChange(
			cli::array<System::Double> ^channels, ServiceClass sc);
	
	private:
		::vrpn_Analog_Output_Remote *m_analogOut;
		
		void Initialize(System::String ^name, vrpn_Connection *lpConn);
	};
}