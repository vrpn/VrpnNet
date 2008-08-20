#include "stdafx.h"

#include "quat.h"
#include "TrackerRemote.h"

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace Vrpn;

delegate void TrackerChangeCallback(void *userData, const vrpn_TRACKERCB info);
delegate void TrackerVelocityCallback(void *userData, const vrpn_TRACKERVELCB info);
delegate void TrackerAccelCallback(void *userData, const vrpn_TRACKERACCCB info);

TrackerRemote::TrackerRemote(String ^name)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);

	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());
	m_tracker = new ::vrpn_Tracker_Remote(ansiName);

	Marshal::FreeHGlobal(hAnsiName);

	RegisterHandlers();
}

TrackerRemote::TrackerRemote(String ^name, Connection ^connection)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);

	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());
	m_tracker = new ::vrpn_Tracker_Remote(ansiName, connection->ToPointer());

	Marshal::FreeHGlobal(hAnsiName);

	RegisterHandlers();
}

TrackerRemote::~TrackerRemote()
{
	delete m_tracker;

	gc_positionChange.Free();
	gc_velocityChange.Free();
	gc_accelChange.Free();
}

void TrackerRemote::Update()
{
	m_tracker->mainloop();
}

void TrackerRemote::MuteWarnings::set(Boolean shutUp)
{
	m_tracker->shutup = shutUp;
}

Boolean TrackerRemote::MuteWarnings::get()
{
	return m_tracker->shutup;
}

Connection^ TrackerRemote::GetConnection()
{
	return Connection::FromPointer(m_tracker->connectionPtr());
}

void TrackerRemote::RequestTrackerToRoomXform()
{
	if (m_tracker->request_t2r_xform())
		throw gcnew VrpnException();
}

void TrackerRemote::RequestUnitToSensorXform()
{
	if (m_tracker->request_u2s_xform())
		throw gcnew VrpnException();
}

void TrackerRemote::RequestWorkspaceBounds()
{
	if (m_tracker->request_workspace())
		throw gcnew VrpnException();
}

void TrackerRemote::UpdateRate::set(Double samplesPerSecond)
{
	if (m_tracker->set_update_rate(samplesPerSecond))
		throw gcnew VrpnException();
}

void TrackerRemote::ResetOrigin()
{
	if (m_tracker->reset_origin())
		throw gcnew VrpnException();
}

void TrackerRemote::RegisterHandlers()
{
	// Register change handler
	TrackerChangeCallback ^poscb = gcnew TrackerChangeCallback(this, &TrackerRemote::onPositionChange);
	gc_positionChange = GCHandle::Alloc(poscb);
	IntPtr pPosChange = Marshal::GetFunctionPointerForDelegate(poscb);
	vrpn_TRACKERCHANGEHANDLER pPosChangeHandler = static_cast<vrpn_TRACKERCHANGEHANDLER>(pPosChange.ToPointer());
	m_tracker->register_change_handler(0, pPosChangeHandler);

	// Register velocity handler
	TrackerVelocityCallback ^velcb = gcnew TrackerVelocityCallback(this, &TrackerRemote::onVelocityChange);
	gc_velocityChange = GCHandle::Alloc(velcb);
	IntPtr pVelChange = Marshal::GetFunctionPointerForDelegate(velcb);
	vrpn_TRACKERVELCHANGEHANDLER pVelChangeHandler = static_cast<vrpn_TRACKERVELCHANGEHANDLER>(pVelChange.ToPointer());
	m_tracker->register_change_handler(0, pVelChangeHandler);

	// Register acceleration handler
	TrackerAccelCallback ^acccb = gcnew TrackerAccelCallback(this, &TrackerRemote::onAccelChange);
	gc_accelChange = GCHandle::Alloc(acccb);
	IntPtr pAccChange = Marshal::GetFunctionPointerForDelegate(acccb);
	vrpn_TRACKERACCCHANGEHANDLER pAccChangeHandler = static_cast<vrpn_TRACKERACCCHANGEHANDLER>(pAccChange.ToPointer());
	m_tracker->register_change_handler(0, pAccChangeHandler);
}

void TrackerRemote::onPositionChange(void *userData, const vrpn_TRACKERCB info)
{
	TrackerChangeEventArgs ^e = gcnew TrackerChangeEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Sensor = info.sensor;
	e->Position = Vector3(info.pos[0], info.pos[1], info.pos[2]);
	e->Orientation = Quaternion(info.quat[Q_X], info.quat[Q_Y], info.quat[Q_Z], info.quat[Q_W]);

	PositionChanged(this, e);
}

void TrackerRemote::onVelocityChange(void *userData, const vrpn_TRACKERVELCB info)
{
	TrackerVelocityChangeEventArgs ^e = gcnew TrackerVelocityChangeEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Sensor = info.sensor;
	e->Velocity = Vector3(info.vel[0], info.vel[1], info.vel[2]);
	e->VelocityQuat = Quaternion(info.vel_quat[Q_X], info.vel_quat[Q_Y], info.vel_quat[Q_Z], info.vel_quat[Q_W]);
	e->VelocityQuatDeltaSeconds = info.vel_quat_dt;

	VelocityChanged(this, e);
}

void TrackerRemote::onAccelChange(void *userData, const vrpn_TRACKERACCCB info)
{
	TrackerAccelChangeEventArgs ^e = gcnew TrackerAccelChangeEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Sensor = info.sensor;
	e->Acceleration = Vector3(info.acc[0], info.acc[1], info.acc[2]);
	e->AccelerationQuat = Quaternion(info.acc_quat[Q_X], info.acc_quat[Q_Y], info.acc_quat[Q_Z], info.acc_quat[Q_W]);
	e->AccelerationQuatDeltaSeconds = info.acc_quat_dt;

	AccelerationChanged(this, e);
}
