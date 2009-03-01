// TrackerServer.cpp: Implementation of Vrpn.TrackerServer
//
// Copyright (c) 2008-2009 Chris VanderKnyff
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
#include "TrackerServer.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;

TrackerServer::TrackerServer(System::String ^name, Vrpn::Connection ^connection, int numSensors)
{
	Initialize(name, connection, numSensors);
}

TrackerServer::TrackerServer(System::String ^name, Vrpn::Connection ^connection)
{
	Initialize(name, connection, 1);
}

void TrackerServer::Initialize(System::String ^name, Vrpn::Connection ^connection, int numSensors)
{
	m_disposed = false;
	
	IntPtr hName = Marshal::StringToHGlobalAnsi(name);
	const char *cName = static_cast<const char *>(hName.ToPointer());
	m_server = new vrpn_Tracker_Server(cName, connection->ToPointer(), numSensors);
	Marshal::FreeHGlobal(hName);
}

TrackerServer::~TrackerServer()
{
	this->!TrackerServer();
}

TrackerServer::!TrackerServer()
{
	delete m_server;
	m_disposed = true;
}

void TrackerServer::Update()
{
	CHECK_DISPOSAL_STATUS();
	m_server->mainloop();
}

Connection ^TrackerServer::GetConnection()
{
	CHECK_DISPOSAL_STATUS();
	return Connection::FromPointer(m_server->connectionPtr());
}

bool TrackerServer::MuteWarnings::get()
{
	CHECK_DISPOSAL_STATUS();
	return m_server->shutup;
}

void TrackerServer::MuteWarnings::set(bool shutUp)
{
	CHECK_DISPOSAL_STATUS();
	m_server->shutup = shutUp;
}

void TrackerServer::ReportPose(int sensor, System::DateTime time, 
	Vrpn::Vector3 position, Vrpn::Quaternion quaternion)
{
	ReportPose(sensor, time, position, quaternion, ServiceClass::LowLatency);
}

void TrackerServer::ReportPose(int sensor, System::DateTime time, 
	Vrpn::Vector3 position, Vrpn::Quaternion quaternion, Vrpn::ServiceClass classOfService)
{
	CHECK_DISPOSAL_STATUS();

	timeval tm;
	double posArray[3];
	double quatArray[4];

	VrpnUtils::CreateTimeval(time, &tm);
	VrpnUtils::CreateVector(position, posArray);
	VrpnUtils::CreateQuat(quaternion, quatArray);

	m_server->report_pose(sensor, tm, posArray, quatArray, safe_cast<vrpn_uint32>(classOfService));
}

void TrackerServer::ReportPoseVelocity(int sensor, System::DateTime time, 
	Vrpn::Vector3 position, Vrpn::Quaternion quaternion, double interval)
{
	ReportPoseVelocity(sensor, time, position, quaternion, interval, ServiceClass::LowLatency);
}

void TrackerServer::ReportPoseVelocity(int sensor, System::DateTime time, 
	Vrpn::Vector3 position, Vrpn::Quaternion quaternion, double interval, Vrpn::ServiceClass classOfService)
{
	CHECK_DISPOSAL_STATUS();

	timeval tm;
	double posArray[3];
	double quatArray[4];

	VrpnUtils::CreateTimeval(time, &tm);
	VrpnUtils::CreateVector(position, posArray);
	VrpnUtils::CreateQuat(quaternion, quatArray);

	m_server->report_pose_velocity(sensor, tm, posArray, quatArray, 
		interval, safe_cast<vrpn_uint32>(classOfService));
}

void TrackerServer::ReportPoseAcceleration(int sensor, System::DateTime time, 
	Vrpn::Vector3 position, Vrpn::Quaternion quaternion, double interval)
{
	ReportPoseAcceleration(sensor, time, position, quaternion, interval, ServiceClass::LowLatency);
}

void TrackerServer::ReportPoseAcceleration(int sensor, System::DateTime time, 
	Vrpn::Vector3 position, Vrpn::Quaternion quaternion, double interval, Vrpn::ServiceClass classOfService)
{
	CHECK_DISPOSAL_STATUS();

	timeval tm;
	double posArray[3];
	double quatArray[4];

	VrpnUtils::CreateTimeval(time, &tm);
	VrpnUtils::CreateVector(position, posArray);
	VrpnUtils::CreateQuat(quaternion, quatArray);

	m_server->report_pose_acceleration(sensor, tm, posArray, quatArray, 
		interval, safe_cast<vrpn_uint32>(classOfService));
}