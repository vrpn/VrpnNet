// TrackerRemote.h: Interface description for Vrpn.TrackerRemote
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

#include "vrpn_Tracker.h"

#include "BaseTypes.h"
#include "Connection.h"

namespace Vrpn {
	public ref class TrackerChangeEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property System::Int32 Sensor;
		property Vrpn::Vector3 Position;
		property Vrpn::Quaternion Orientation;
	};

	public ref class TrackerVelocityChangeEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property System::Int32 Sensor;
		property Vrpn::Vector3 Velocity;
		property Vrpn::Quaternion VelocityQuat;
		property double VelocityQuatDeltaSeconds;
	};

	public ref class TrackerAccelChangeEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property System::Int32 Sensor;
		property Vrpn::Vector3 Acceleration;
		property Vrpn::Quaternion AccelerationQuat;
		property double AccelerationQuatDeltaSeconds;
	};

	public ref class TrackerToRoomEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property Vrpn::Vector3 Position;
		property Vrpn::Quaternion Orientation;
	};

	public ref class TrackerUnitToSensorEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property System::Int32 Sensor;
		property Vrpn::Vector3 Position;
		property Vrpn::Quaternion Orientation;
	};

	public ref class TrackerWorkspaceBoundsEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property Vrpn::Vector3 WorkspaceMin;
		property Vrpn::Vector3 WorkspaceMax;
	};

	public delegate void TrackerChangeEventHandler(System::Object ^sender, TrackerChangeEventArgs ^e);
	public delegate void TrackerVelocityChangeEventHandler(System::Object ^sender, TrackerVelocityChangeEventArgs ^e);
	public delegate void TrackerAccelChangeEventHandler(System::Object ^sender, TrackerAccelChangeEventArgs ^e);
	public delegate void TrackerToRoomChangeEventHandler(System::Object ^sender, TrackerToRoomEventArgs ^e);
	public delegate void TrackerUnitToSensorChangeEventHandler(System::Object ^sender, TrackerUnitToSensorEventArgs ^e);
	public delegate void TrackerWorkspaceBoundsChangeEventHandler(System::Object ^sender, TrackerWorkspaceBoundsEventArgs ^e);

	public ref class TrackerRemote: public Vrpn::IVrpnObject
	{
	public:
		TrackerRemote(System::String ^name);
		TrackerRemote(System::String ^name, Vrpn::Connection ^connection);
		~TrackerRemote();

		virtual void Update(); // from IVrpnObject
		virtual Connection^ GetConnection(); // from IVrpnObject
		property System::Boolean MuteWarnings // from IVrpnObject
		{
			virtual void set(System::Boolean shutUp);
			virtual System::Boolean get();
		}

		void RequestTrackerToRoomXform();
		void RequestUnitToSensorXform();
		void RequestWorkspaceBounds();

		property double UpdateRate
		{
			void set(double samplesPerSecond);
		}

		void ResetOrigin();

		event TrackerChangeEventHandler^ PositionChanged;
		event TrackerVelocityChangeEventHandler^ VelocityChanged;
		event TrackerAccelChangeEventHandler^ AccelerationChanged;
		event TrackerToRoomChangeEventHandler^ TrackerToRoomChanged;
		event TrackerUnitToSensorChangeEventHandler^ UnitToSensorChanged;
		event TrackerWorkspaceBoundsChangeEventHandler^ WorkspaceBoundsChanged;

	private:
		::vrpn_Tracker_Remote *m_tracker;

		void RegisterHandlers();

		// Callbacks
		void onPositionChange(void *userData, const vrpn_TRACKERCB info);
		void onVelocityChange(void *userData, const vrpn_TRACKERVELCB info);
		void onAccelChange(void *userData, const vrpn_TRACKERACCCB info);
		void onT2RChange(void *userData, const vrpn_TRACKERTRACKER2ROOMCB info);
		void onU2SChange(void *userData, const vrpn_TRACKERUNIT2SENSORCB info);
		void onBoundsChange(void *userData, const vrpn_TRACKERWORKSPACECB info);

		// References to VRPN callback delegates
		System::Runtime::InteropServices::GCHandle gc_positionChange;
		System::Runtime::InteropServices::GCHandle gc_velocityChange;
		System::Runtime::InteropServices::GCHandle gc_accelChange;
		System::Runtime::InteropServices::GCHandle gc_t2rChange;
		System::Runtime::InteropServices::GCHandle gc_u2sChange;
		System::Runtime::InteropServices::GCHandle gc_boundsChange;
	};
}
