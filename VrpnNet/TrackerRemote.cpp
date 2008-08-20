#include "stdafx.h"

#include "quat.h"
#include "TrackerRemote.h"

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace Vrpn;

delegate void TrackerChangeCallback(void *userData, const vrpn_TRACKERCB info);
delegate void TrackerVelocityCallback(void *userData, const vrpn_TRACKERVELCB info);
delegate void TrackerAccelCallback(void *userData, const vrpn_TRACKERACCCB info);
delegate void TrackerToRoomCallback(void *userData, const vrpn_TRACKERTRACKER2ROOMCB info);
delegate void TrackerUnitToSensorCallback(void *userData, const vrpn_TRACKERUNIT2SENSORCB info);
delegate void TrackerWorkspaceCallback(void *userData, const vrpn_TRACKERWORKSPACECB info);

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
	gc_t2rChange.Free();
	gc_u2sChange.Free();
	gc_boundsChange.Free();
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
	vrpn_TRACKERCHANGEHANDLER pPosChangeHandler = 
		static_cast<vrpn_TRACKERCHANGEHANDLER>(pPosChange.ToPointer());
	m_tracker->register_change_handler(0, pPosChangeHandler);

	// Register velocity handler
	TrackerVelocityCallback ^velcb = gcnew TrackerVelocityCallback(this, &TrackerRemote::onVelocityChange);
	gc_velocityChange = GCHandle::Alloc(velcb);
	IntPtr pVelChange = Marshal::GetFunctionPointerForDelegate(velcb);
	vrpn_TRACKERVELCHANGEHANDLER pVelChangeHandler = 
		static_cast<vrpn_TRACKERVELCHANGEHANDLER>(pVelChange.ToPointer());
	m_tracker->register_change_handler(0, pVelChangeHandler);

	// Register acceleration handler
	TrackerAccelCallback ^acccb = gcnew TrackerAccelCallback(this, &TrackerRemote::onAccelChange);
	gc_accelChange = GCHandle::Alloc(acccb);
	IntPtr pAccChange = Marshal::GetFunctionPointerForDelegate(acccb);
	vrpn_TRACKERACCCHANGEHANDLER pAccChangeHandler = 
		static_cast<vrpn_TRACKERACCCHANGEHANDLER>(pAccChange.ToPointer());
	m_tracker->register_change_handler(0, pAccChangeHandler);

	// Register tracker-to-room handler
	TrackerToRoomCallback ^t2rcb = gcnew TrackerToRoomCallback(this, &TrackerRemote::onT2RChange);
	gc_t2rChange = GCHandle::Alloc(t2rcb);
	IntPtr pT2RChange = Marshal::GetFunctionPointerForDelegate(t2rcb);
	vrpn_TRACKERTRACKER2ROOMCHANGEHANDLER pT2RChangeHandler = 
		static_cast<vrpn_TRACKERTRACKER2ROOMCHANGEHANDLER>(pT2RChange.ToPointer());
	m_tracker->register_change_handler(0, pT2RChangeHandler);

	// Register unit-to-sensor handler
	TrackerUnitToSensorCallback ^u2scb = gcnew TrackerUnitToSensorCallback(this, &TrackerRemote::onU2SChange);
	gc_u2sChange = GCHandle::Alloc(u2scb);
	IntPtr pU2SChange = Marshal::GetFunctionPointerForDelegate(u2scb);
	vrpn_TRACKERUNIT2SENSORCHANGEHANDLER pU2SChangeHandler =
		static_cast<vrpn_TRACKERUNIT2SENSORCHANGEHANDLER>(pU2SChange.ToPointer());
	m_tracker->register_change_handler(0, pU2SChangeHandler);

	// Register workspace handler
	TrackerWorkspaceCallback ^wcb = gcnew TrackerWorkspaceCallback(this, &TrackerRemote::onBoundsChange);
	gc_boundsChange = GCHandle::Alloc(wcb);
	IntPtr pBoundsChange = Marshal::GetFunctionPointerForDelegate(wcb);
	vrpn_TRACKERWORKSPACECHANGEHANDLER pBoundsHandler = 
		static_cast<vrpn_TRACKERWORKSPACECHANGEHANDLER>(pBoundsChange.ToPointer());
	m_tracker->register_change_handler(0, pBoundsHandler);
}

void TrackerRemote::onPositionChange(void *, const vrpn_TRACKERCB info)
{
	TrackerChangeEventArgs ^e = gcnew TrackerChangeEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Sensor = info.sensor;
	e->Position = Vector3(info.pos[0], info.pos[1], info.pos[2]);
	e->Orientation = Quaternion(info.quat[Q_X], info.quat[Q_Y], 
		info.quat[Q_Z], info.quat[Q_W]);

	PositionChanged(this, e);
}

void TrackerRemote::onVelocityChange(void *, const vrpn_TRACKERVELCB info)
{
	TrackerVelocityChangeEventArgs ^e = gcnew TrackerVelocityChangeEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Sensor = info.sensor;
	e->Velocity = Vector3(info.vel[0], info.vel[1], info.vel[2]);
	e->VelocityQuat = Quaternion(info.vel_quat[Q_X], info.vel_quat[Q_Y], 
		info.vel_quat[Q_Z], info.vel_quat[Q_W]);
	e->VelocityQuatDeltaSeconds = info.vel_quat_dt;

	VelocityChanged(this, e);
}

void TrackerRemote::onAccelChange(void *, const vrpn_TRACKERACCCB info)
{
	TrackerAccelChangeEventArgs ^e = gcnew TrackerAccelChangeEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Sensor = info.sensor;
	e->Acceleration = Vector3(info.acc[0], info.acc[1], info.acc[2]);
	e->AccelerationQuat = Quaternion(info.acc_quat[Q_X], info.acc_quat[Q_Y], 
		info.acc_quat[Q_Z], info.acc_quat[Q_W]);
	e->AccelerationQuatDeltaSeconds = info.acc_quat_dt;

	AccelerationChanged(this, e);
}

void TrackerRemote::onT2RChange(void *, const vrpn_TRACKERTRACKER2ROOMCB info)
{
	TrackerToRoomEventArgs ^e = gcnew TrackerToRoomEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Position = Vector3(info.tracker2room[0], info.tracker2room[1], info.tracker2room[2]);
	e->Orientation = Quaternion(info.tracker2room_quat[Q_X], info.tracker2room_quat[Q_Y],
		info.tracker2room_quat[Q_Z], info.tracker2room_quat[Q_W]);
}

void TrackerRemote::onU2SChange(void *, const vrpn_TRACKERUNIT2SENSORCB info)
{
	TrackerUnitToSensorEventArgs ^e = gcnew TrackerUnitToSensorEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Sensor = info.sensor;
	e->Position = Vector3(info.unit2sensor[0], info.unit2sensor[1], info.unit2sensor[2]);
	e->Orientation = Quaternion(info.unit2sensor_quat[Q_X], info.unit2sensor_quat[Q_Y], 
		info.unit2sensor_quat[Q_Z], info.unit2sensor_quat[Q_W]);

	UnitToSensorChanged(this, e);
}
