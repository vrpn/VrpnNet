// TrackerRemote.cpp: Implementation for Vrpn.TrackerRemote
//
// Copyright (c) 2008 Chris VanderKnyff
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "stdafx.h"

#include "quat.h"
#include "TrackerRemote.h"

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace Vrpn;

namespace {
	delegate void TrackerChangeCallback(void *userData, const vrpn_TRACKERCB info);
	delegate void TrackerVelocityCallback(void *userData, const vrpn_TRACKERVELCB info);
	delegate void TrackerAccelCallback(void *userData, const vrpn_TRACKERACCCB info);
	delegate void TrackerToRoomCallback(void *userData, const vrpn_TRACKERTRACKER2ROOMCB info);
	delegate void TrackerUnitToSensorCallback(void *userData, const vrpn_TRACKERUNIT2SENSORCB info);
	delegate void TrackerWorkspaceCallback(void *userData, const vrpn_TRACKERWORKSPACECB info);
}

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
	e->Position = VrpnUtils::ConvertVector(info.pos);
	e->Orientation = VrpnUtils::ConvertQuat(info.quat);

	PositionChanged(this, e);
}

void TrackerRemote::onVelocityChange(void *, const vrpn_TRACKERVELCB info)
{
	TrackerVelocityChangeEventArgs ^e = gcnew TrackerVelocityChangeEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Sensor = info.sensor;
	e->Velocity = VrpnUtils::ConvertVector(info.vel);
	e->VelocityQuat = VrpnUtils::ConvertQuat(info.vel_quat);
	e->VelocityQuatDeltaSeconds = info.vel_quat_dt;

	VelocityChanged(this, e);
}

void TrackerRemote::onAccelChange(void *, const vrpn_TRACKERACCCB info)
{
	TrackerAccelChangeEventArgs ^e = gcnew TrackerAccelChangeEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Sensor = info.sensor;
	e->Acceleration = VrpnUtils::ConvertVector(info.acc);
	e->AccelerationQuat = VrpnUtils::ConvertQuat(info.acc_quat);
	e->AccelerationQuatDeltaSeconds = info.acc_quat_dt;

	AccelerationChanged(this, e);
}

void TrackerRemote::onT2RChange(void *, const vrpn_TRACKERTRACKER2ROOMCB info)
{
	TrackerToRoomEventArgs ^e = gcnew TrackerToRoomEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Position = VrpnUtils::ConvertVector(info.tracker2room);
	e->Orientation = VrpnUtils::ConvertQuat(info.tracker2room_quat);
}

void TrackerRemote::onU2SChange(void *, const vrpn_TRACKERUNIT2SENSORCB info)
{
	TrackerUnitToSensorEventArgs ^e = gcnew TrackerUnitToSensorEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Sensor = info.sensor;
	e->Position = VrpnUtils::ConvertVector(info.unit2sensor);
	e->Orientation = VrpnUtils::ConvertQuat(info.unit2sensor_quat);

	UnitToSensorChanged(this, e);
}

void TrackerRemote::onBoundsChange(void *, const vrpn_TRACKERWORKSPACECB info)
{
	TrackerWorkspaceBoundsEventArgs ^e = gcnew TrackerWorkspaceBoundsEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->WorkspaceMin = VrpnUtils::ConvertVector(info.workspace_min);
	e->WorkspaceMax = VrpnUtils::ConvertVector(info.workspace_max);

	WorkspaceBoundsChanged(this, e);
}