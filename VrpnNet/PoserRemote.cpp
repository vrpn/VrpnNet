#include "stdafx.h"
#include "PoserRemote.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;

PoserRemote::PoserRemote(String ^name)
{
	Initialize(name, 0);
}

PoserRemote::PoserRemote(System::String ^name, Vrpn::Connection ^connection)
{
	Initialize(name, connection->ToPointer());
}

PoserRemote::~PoserRemote()
{
	delete m_poser;
}

void PoserRemote::Initialize(System::String ^name, vrpn_Connection *lpConn)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);
	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());

	m_poser = new ::vrpn_Poser_Remote(ansiName, lpConn);

	Marshal::FreeHGlobal(hAnsiName);
}

void PoserRemote::Update()
{
	m_poser->mainloop();
}

Connection^ PoserRemote::GetConnection()
{
	return Connection::FromPointer(m_poser->connectionPtr());
}

void PoserRemote::MuteWarnings::set(Boolean shutUp)
{
	m_poser->shutup = shutUp;
}

Boolean PoserRemote::MuteWarnings::get()
{
	return m_poser->shutup;
}

void PoserRemote::RequestPose(System::DateTime time, Vrpn::Vector3 position, Vrpn::Quaternion quaternion)
{
	struct timeval cTime = {0, 0};
	VrpnUtils::CreateTimeval(time, &cTime);

	double pos[3], quat[4];
	VrpnUtils::CreateVector(position, pos);
	VrpnUtils::CreateQuat(quaternion, quat);

	int result = m_poser->request_pose(cTime, pos, quat);
	if (!result)
		throw gcnew VrpnException();
}

void PoserRemote::RequestPoseRelative(System::DateTime time, Vrpn::Vector3 positionDelta, Vrpn::Quaternion quaternion)
{
	struct timeval cTime = {0, 0};
	VrpnUtils::CreateTimeval(time, &cTime);

	double pos[3], quat[4];
	VrpnUtils::CreateVector(positionDelta, pos);
	VrpnUtils::CreateQuat(quaternion, quat);

	int result = m_poser->request_pose_relative(cTime, pos, quat);
	if (!result)
		throw gcnew VrpnException();
}

void PoserRemote::RequestPoseVelocity(System::DateTime time, Vrpn::Vector3 velocity, Vrpn::Quaternion quaternion, double interval)
{
	struct timeval cTime = {0, 0};
	VrpnUtils::CreateTimeval(time, &cTime);

	double vel[3], quat[4];
	VrpnUtils::CreateVector(velocity, vel);
	VrpnUtils::CreateQuat(quaternion, quat);

	int result = m_poser->request_pose_velocity(cTime, vel, quat, interval);
	if (!result)
		throw gcnew VrpnException();
}

void PoserRemote::RequestPoseVelocityRelative(System::DateTime time, Vrpn::Vector3 velocityDelta, Vrpn::Quaternion quaternion, double intervalDelta)
{
	struct timeval cTime = {0, 0};
	VrpnUtils::CreateTimeval(time, &cTime);

	double vel[3], quat[4];
	VrpnUtils::CreateVector(velocityDelta, vel);
	VrpnUtils::CreateQuat(quaternion, quat);

	int result = m_poser->request_pose_velocity_relative(cTime, vel, quat, intervalDelta);
	if (!result)
		throw gcnew VrpnException();
}