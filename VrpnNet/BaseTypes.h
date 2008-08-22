#pragma once

#include "vrpn_Connection.h"

namespace Vrpn {
	public ref class VrpnException: public System::Exception
	{
	};

	ref class Connection;

	public interface class IVrpnObject
	{
		void Update();

		Connection^ GetConnection();

		property System::Boolean MuteWarnings
		{
			void set(System::Boolean shutUp);
			System::Boolean get();
		}
	};

	[System::Diagnostics::DebuggerDisplay("Vector3 ({X}, {Y}, {Z})")]
	public value class Vector3
	{
	public:
		Vector3(System::Double x, System::Double y, System::Double z);

		property System::Double X;
		property System::Double Y;
		property System::Double Z;
	};

	[System::Diagnostics::DebuggerDisplay("Quaternion ({X}, {Y}, {Z}, {W})")]
	public value class Quaternion
	{
	public:
		Quaternion(System::Double x, System::Double y, System::Double z, System::Double w);

		property System::Double X;
		property System::Double Y;
		property System::Double Z;
		property System::Double W;
	};

	ref class VrpnUtils
	{
	public:
		static System::DateTime ConvertTimeval(struct timeval tm);
		static Vrpn::Vector3 ConvertVector(const double vec[3]);
		static Vrpn::Quaternion ConvertQuat(const double quat[4]);
	
	private:
		VrpnUtils();
	};
}