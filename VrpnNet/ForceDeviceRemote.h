// ForceDeviceRemote.h: Interface description for Vrpn.ForceDeviceRemote
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
		property System::Windows::Media::Media3D::Vector3D Position;
		property System::Windows::Media::Media3D::Quaternion Orientation;
	};

	public ref class ForceChangeEventArgs: public System::EventArgs
	{
	public:
		property System::DateTime Time;
		property System::Windows::Media::Media3D::Vector3D Force;
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
		!ForceDeviceRemote();

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

		void SetVertex(System::Int32 vertexNumber, System::Windows::Media::Media3D::Vector3D point);
		void SetNormal(System::Int32 normalNumber, System::Windows::Media::Media3D::Vector3D normal);
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
		void SetObjectVertex(System::Int32 objectNumber, System::Int32 vertexNumber, System::Windows::Media::Media3D::Vector3D point);
		void SetObjectNormal(System::Int32 objectNumber, System::Int32 normalNumber, System::Windows::Media::Media3D::Vector3D normal);
		void SetObjectTriangle(System::Int32 objectNumber, System::Int32 triangleNumber, 
			System::Int32 vertex1, System::Int32 vertex2, System::Int32 vertex3);
		void SetObjectTriangle(System::Int32 objectNumber, System::Int32 triangleNumber, 
			System::Int32 vertex1, System::Int32 vertex2, System::Int32 vertex3,
			System::Int32 normal1, System::Int32 normal2, System::Int32 normal3);
		void RemoveObjectTriangle(System::Int32 objectNumber, System::Int32 triangleNumber);

		void UpdateObjectTrimeshChanges(System::Int32 objectNumber);
		void SetObjectTrimeshTransform(System::Int32 objectNumber, cli::array<System::Single> ^matrix);
		void SetObjectPosition(System::Int32 objectNumber, System::Windows::Media::Media3D::Vector3D position);
		void SetObjectOrientation(System::Int32 objectNumber, System::Windows::Media::Media3D::Vector3D axis, System::Single angle);
		void SetObjectScale(System::Int32 objectNumber, System::Windows::Media::Media3D::Vector3D scale);
		void RemoveObject(System::Int32 objectNumber);
		void ClearObjectTrimesh(System::Int32 objectNumber);

		void MoveToParent(System::Int32 objectNumber, System::Int32 parentNumber);
		void SetHapticOrigin(System::Windows::Media::Media3D::Vector3D position, System::Windows::Media::Media3D::Vector3D axis, System::Single angle);
		void SetHapticScale(System::Single scale);
		void SetSceneOrigin(System::Windows::Media::Media3D::Vector3D position, System::Windows::Media::Media3D::Vector3D axis, System::Single angle);
		System::Int32 GetNewObjectId();
		void SetObjectIsTouchable(System::Int32 objectNumber, System::Boolean isTouchable);

		// Wraps useHcollide() and useGhost()
		void SetMeshPacking(Vrpn::ForceDeviceMeshPacking api);

		void EnableConstraints(System::Boolean enable);
		void SetConstraintMode(Vrpn::ConstraintGeometry mode);
		void SetConstraintPoint(System::Windows::Media::Media3D::Vector3D point);
		void SetConstraintLinePoint(System::Windows::Media::Media3D::Vector3D point);
		void SetConstraintLineDirection(System::Windows::Media::Media3D::Vector3D direction);
		void SetConstraintPlanePoint(System::Windows::Media::Media3D::Vector3D point);
		void SetConstraintPlaneNormal(System::Windows::Media::Media3D::Vector3D normal);
		void SetConstraintKSpring(System::Single k);

		void SendForceField();
		void SendForceField(System::Windows::Media::Media3D::Vector3D origin, System::Windows::Media::Media3D::Vector3D force,
			cli::array<System::Single, 2> ^jacobian, System::Single radius);
		void StopForceField();

		void StartEffect();
		void StopEffect();

		event ForceSurfaceContactEventHandler^ SurfaceContact;
		event ForceChangeEventHandler^ ForceChange;
		event ForceErrorEventHandler^ Error;

	private:
		::vrpn_ForceDevice_Remote *m_force;
		System::Boolean m_disposed;

		void Initialize(System::String ^name, ::vrpn_Connection *lpConn);
		void onSurfaceContact(void *userData, const ::vrpn_FORCESCPCB info);
		void onForceChange(void *userData, const ::vrpn_FORCECB info);
		void onErrorReport(void *userData, const ::vrpn_FORCEERRORCB info);

		System::Runtime::InteropServices::GCHandle gc_forceScp;
		System::Runtime::InteropServices::GCHandle gc_forceChange;
		System::Runtime::InteropServices::GCHandle gc_error;
	};
}