#pragma once

#include "vrpn_Connection.h"

namespace Vrpn {
	public ref class VrpnException: public System::Exception
	{
	};

	[System::Flags]
	public enum class ServiceClass
	{
		Reliable = (1 << 0),
		FixedLatency = (1 << 1),
		LowLatency = (1 << 2),
		FixedThroughput = (1 << 3),
		HighThroughput = (1 << 4)
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
		static void CreateTimeval(System::DateTime time, struct timeval *tm);
		static void CreateVector(Vrpn::Vector3 vec, double vArray[]);
		static void CreateVector(Vrpn::Vector3 vec, float vArray[]);
		static void CreateQuat(Vrpn::Quaternion quat, double qArray[]);
	
	private:
		VrpnUtils();
	};
}