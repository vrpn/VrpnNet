// PoserRemote.h

#pragma once

#include "vrpn_Poser.h"
#include "BaseTypes.h"
#include "Connection.h"

namespace Vrpn {
	public ref class PoserRemote: public IVrpnObject
	{
	public:
		PoserRemote(System::String ^name);
		PoserRemote(System::String ^name, Vrpn::Connection ^connection);
		~PoserRemote();

		virtual void Update();
		virtual Vrpn::Connection^ GetConnection();
		property System::Boolean MuteWarnings
		{
			virtual void set(System::Boolean);
			virtual System::Boolean get();
		}

		void RequestPose(System::DateTime time,
			Vrpn::Vector3 position,
			Vrpn::Quaternion quaternion);

		void RequestPoseRelative(System::DateTime time,
			Vrpn::Vector3 positionDelta,
			Vrpn::Quaternion quaternion);

		void RequestPoseVelocity(System::DateTime time,
			Vrpn::Vector3 velocity,
			Vrpn::Quaternion quaternion,
			System::Double interval);

		void RequestPoseVelocityRelative(System::DateTime time,
			Vrpn::Vector3 velocityDelta,
			Vrpn::Quaternion quaternion,
			System::Double intervalDelta);

	private:
		::vrpn_Poser_Remote *m_poser;

		void Initialize(System::String ^name, vrpn_Connection *lpConn);
	};
}