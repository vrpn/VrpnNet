// TrackerRemote.h

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
		property System::Double VelocityQuatDeltaSeconds;
	};

	public ref class TrackerAccelChangeEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property System::Int32 Sensor;
		property Vrpn::Vector3 Acceleration;
		property Vrpn::Quaternion AccelerationQuat;
		property System::Double AccelerationQuatDeltaSeconds;
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

		property System::Double UpdateRate
		{
			void set(System::Double samplesPerSecond);
		}

		void ResetOrigin();

		event TrackerChangeEventHandler^ PositionChanged;
		event TrackerVelocityChangeEventHandler^ VelocityChanged;
		event TrackerAccelChangeEventHandler^ AccelerationChanged;

	private:
		::vrpn_Tracker_Remote *m_tracker;

		void RegisterHandlers();

		// Callbacks
		void onPositionChange(void *userData, const vrpn_TRACKERCB info);
		void onVelocityChange(void *userData, const vrpn_TRACKERVELCB info);
		void onAccelChange(void *userData, const vrpn_TRACKERACCCB info);

		// References to VRPN callback delegates
		System::Runtime::InteropServices::GCHandle gc_positionChange;
		System::Runtime::InteropServices::GCHandle gc_velocityChange;
		System::Runtime::InteropServices::GCHandle gc_accelChange;
	};
}
