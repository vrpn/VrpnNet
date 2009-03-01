// TrackerServer.h: Interface description for TrackerServer
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

#pragma once

#include "BaseTypes.h"
#include "Connection.h"
#include "vrpn_Tracker.h"

namespace Vrpn {
	public ref class TrackerServer: public IVrpnObject
	{
	public:
		TrackerServer(System::String ^name, Vrpn::Connection ^connection);
		TrackerServer(System::String ^name, Vrpn::Connection ^connection, System::Int32 numSensors);
		~TrackerServer();
		!TrackerServer();

		virtual void Update(); // from IVrpnObject
		virtual Vrpn::Connection ^GetConnection(); // from IVrpnObject
		property System::Boolean MuteWarnings // from IVrpnObject
		{
			virtual void set(System::Boolean shutUp);
			virtual System::Boolean get();
		}

		void ReportPose(System::Int32 sensor, System::DateTime time, 
			Vrpn::Vector3 position, Vrpn::Quaternion quaternion);

		void ReportPose(System::Int32 sensor, System::DateTime time, 
			Vrpn::Vector3 position, Vrpn::Quaternion quaternion,
			Vrpn::ServiceClass classOfService);

		void ReportPoseVelocity(System::Int32 sensor, System::DateTime time, 
			Vrpn::Vector3 position, Vrpn::Quaternion quaternion, System::Double interval);

		void ReportPoseVelocity(System::Int32 sensor, System::DateTime time, 
			Vrpn::Vector3 position, Vrpn::Quaternion quaternion, System::Double interval,
			Vrpn::ServiceClass classOfService);

		void ReportPoseAcceleration(System::Int32 sensor, System::DateTime time, 
			Vrpn::Vector3 position, Vrpn::Quaternion quaternion, System::Double interval);

		void ReportPoseAcceleration(System::Int32 sensor, System::DateTime time, 
			Vrpn::Vector3 position, Vrpn::Quaternion quaternion, System::Double interval,
			Vrpn::ServiceClass classOfService);

	private:
		::vrpn_Tracker_Server *m_server;
		System::Boolean m_disposed;

		void Initialize(System::String ^name, Vrpn::Connection ^connection, System::Int32 numSensors);
	};
}