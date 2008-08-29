#pragma once

#include "vrpn_ForceDevice.h"
#include "BaseTypes.h"
#include "Connection.h"

namespace Vrpn {
	public enum class ForceDeviceError
	{
		ValueOutOfRange = 0,
		DutyCycleError = 1,
		ForceError = 2,
		MiscError = 3,
		OK = 4
	};

	public enum class ForceDeviceMeshPacking
	{
		Hcollide,
		Ghost
	};

	public enum class ConstraintGeometry
	{
		NoConstraint = ::vrpn_ForceDevice_Remote::NO_CONSTRAINT,
		PointConstraint = ::vrpn_ForceDevice_Remote::POINT_CONSTRAINT,
		LineConstraint = ::vrpn_ForceDevice_Remote::LINE_CONSTRAINT,
		PlaneConstraint = ::vrpn_ForceDevice_Remote::PLANE_CONSTRAINT,
	};

	public ref class ForceSurfaceContactEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property Vrpn::Vector3 Position;
		property Vrpn::Quaternion Orientation;
	};

	public ref class ForceChangeEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property Vrpn::Vector3 Force;
	};

	public ref class ForceErrorEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property Vrpn::ForceDeviceError Error;
	};

	public delegate void ForceSurfaceContactEventHandler(System::Object ^sender,
		ForceSurfaceContactEventArgs ^e);

	public delegate void ForceChangeEventHandler(System::Object ^sender,
		ForceChangeEventArgs ^e);

	public delegate void ForceErrorEventHandler(System::Object ^sender,
		ForceErrorEventArgs ^e);

	public ref class ForceDeviceRemote: public IVrpnObject
	{
	public:
		ForceDeviceRemote(System::String ^name);
		ForceDeviceRemote(System::String ^name, Vrpn::Connection ^connection);
		~ForceDeviceRemote();

		virtual void Update(); // from IVrpnObject
		virtual Connection^ GetConnection(); // from IVrpnObject
		property System::Boolean MuteWarnings // from IVrpnObject
		{
			virtual void set(System::Boolean);
			virtual System::Boolean get();
		}

		static property System::Boolean SpringsAreForceFields
		{
			System::Boolean get();
		}

		void SendSurface();
		void StartSurface();
		void StopSurface();

		void SetVertex(System::Int32 vertexNumber, Vrpn::Vector3 point);
		void SetNormal(System::Int32 normalNumber, Vrpn::Vector3 normal);
		void SetTriangle(System::Int32 triangleNumber, System::Int32 vertex1, 
			System::Int32 vertex2, System::Int32 vertex3);
		void SetTriangle(System::Int32 triangleNumber, System::Int32 vertex1, 
			System::Int32 vertex2, System::Int32 vertex3, System::Int32 normal1, 
			System::Int32 normal2, System::Int32 normal3);
		void RemoveTriangle(System::Int32 triangleNumber);

		void UpdateTrimeshChanges();
		void SetTrimeshTransform(cli::array<System::Single> ^matrix);
		void ClearTrimesh();

		void AddObject(System::Int32 objectNumber);
		void AddObject(System::Int32 objectNumber, System::Int32 parentNumber);
		void AddObjectExScene(System::Int32 objectNumber);
		void SetObjectVertex(System::Int32 objectNumber, System::Int32 vertexNumber, Vrpn::Vector3 point);
		void SetObjectNormal(System::Int32 objectNumber, System::Int32 normalNumber, Vrpn::Vector3 normal);
		void SetObjectTriangle(System::Int32 objectNumber, System::Int32 triangleNumber, 
			System::Int32 vertex1, System::Int32 vertex2, System::Int32 vertex3);
		void SetObjectTriangle(System::Int32 objectNumber, System::Int32 triangleNumber, 
			System::Int32 vertex1, System::Int32 vertex2, System::Int32 vertex3,
			System::Int32 normal1, System::Int32 normal2, System::Int32 normal3);
		void RemoveObjectTriangle(System::Int32 objectNumber, System::Int32 triangleNumber);

		void UpdateObjectTrimeshChanges(System::Int32 objectNumber);
		void SetObjectTrimeshTransform(System::Int32 objectNumber, cli::array<System::Single> ^matrix);
		void SetObjectPosition(System::Int32 objectNumber, Vrpn::Vector3 position);
		void SetObjectOrientation(System::Int32 objectNumber, Vrpn::Vector3 axis, System::Single angle);
		void SetObjectScale(System::Int32 objectNumber, Vrpn::Vector3 scale);
		void RemoveObject(System::Int32 objectNumber);
		void ClearObjectTrimesh(System::Int32 objectNumber);

		void MoveToParent(System::Int32 objectNumber, System::Int32 parentNumber);
		void SetHapticOrigin(Vrpn::Vector3 position, Vrpn::Vector3 axis, System::Single angle);
		void SetHapticScale(System::Single scale);
		void SetSceneOrigin(Vrpn::Vector3 position, Vrpn::Vector3 axis, System::Single angle);
		System::Int32 GetNewObjectId();
		void SetObjectIsTouchable(System::Int32 objectNumber, System::Boolean isTouchable);

		// Wraps useHcollide() and useGhost()
		void SetMeshPacking(Vrpn::ForceDeviceMeshPacking api);

		void EnableConstraints(System::Boolean enable);
		void SetConstraintMode(Vrpn::ConstraintGeometry mode);
		void SetConstraintPoint(Vrpn::Vector3 point);
		void SetConstraintLinePoint(Vrpn::Vector3 point);
		void SetConstraintLineDirection(Vrpn::Vector3 direction);
		void SetConstraintPlanePoint(Vrpn::Vector3 point);
		void SetConstraintPlaneNormal(Vrpn::Vector3 normal);
		void SetConstraintKSpring(System::Single k);

		void SendForceField();
		void SendForceField(Vrpn::Vector3 origin, Vrpn::Vector3 force,
			cli::array<System::Single, 2> ^jacobian, System::Single radius);
		void StopForceField();

		void StartEffect();
		void StopEffect();

		event ForceSurfaceContactEventHandler^ SurfaceContact;
		event ForceChangeEventHandler^ ForceChange;
		event ForceErrorEventHandler^ Error;

	private:
		::vrpn_ForceDevice_Remote *m_force;

		void Initialize(System::String ^name, ::vrpn_Connection *lpConn);
		void onSurfaceContact(void *userData, const ::vrpn_FORCESCPCB info);
		void onForceChange(void *userData, const ::vrpn_FORCECB info);
		void onErrorReport(void *userData, const ::vrpn_FORCEERRORCB info);

		System::Runtime::InteropServices::GCHandle gc_forceScp;
		System::Runtime::InteropServices::GCHandle gc_forceChange;
		System::Runtime::InteropServices::GCHandle gc_error;
	};
}