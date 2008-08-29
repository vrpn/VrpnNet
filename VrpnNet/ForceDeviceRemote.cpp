#include "stdafx.h"
#include "ForceDeviceRemote.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Vrpn;

namespace {
	delegate void ForceChangeCallback(void *, const vrpn_FORCECB);
	delegate void ForceSurfaceContactCallback(void *, const vrpn_FORCESCPCB);
	delegate void ForceErrorCallback(void *, const vrpn_FORCEERRORCB);
}

ForceDeviceRemote::ForceDeviceRemote(System::String ^name)
{
	Initialize(name, 0);
}

ForceDeviceRemote::ForceDeviceRemote(System::String ^name, Vrpn::Connection ^connection)
{
	Initialize(name, connection->ToPointer());
}

ForceDeviceRemote::~ForceDeviceRemote()
{
	delete m_force;

	gc_forceScp.Free();
	gc_forceChange.Free();
	gc_error.Free();
}

void ForceDeviceRemote::Initialize(System::String ^name, ::vrpn_Connection *lpConn)
{
	IntPtr hAnsiName = Marshal::StringToHGlobalAnsi(name);
	const char *ansiName = static_cast<const char *>(hAnsiName.ToPointer());

	m_force = new ::vrpn_ForceDevice_Remote(ansiName, lpConn);

	Marshal::FreeHGlobal(hAnsiName);

	// FORCECHANGEHANDLER, FORCESCPHANDLER, FORCEERRORHANDLER
	ForceChangeCallback ^forcecb = gcnew ForceChangeCallback(this, &ForceDeviceRemote::onForceChange);
	gc_forceChange = GCHandle::Alloc(forcecb);
	IntPtr pForceChange = Marshal::GetFunctionPointerForDelegate(forcecb);
	::vrpn_FORCECHANGEHANDLER pForceCallback = 
		static_cast<::vrpn_FORCECHANGEHANDLER>(pForceChange.ToPointer());

	ForceSurfaceContactCallback ^scpcb = gcnew ForceSurfaceContactCallback(this, &ForceDeviceRemote::onSurfaceContact);
	gc_forceScp = GCHandle::Alloc(scpcb);
	IntPtr pForceScp = Marshal::GetFunctionPointerForDelegate(scpcb);
	::vrpn_FORCESCPHANDLER pScpCallback =
		static_cast<::vrpn_FORCESCPHANDLER>(pForceScp.ToPointer());

	ForceErrorCallback ^errorcb = gcnew ForceErrorCallback(this, &ForceDeviceRemote::onErrorReport);
	gc_error = GCHandle::Alloc(errorcb);
	IntPtr pError = Marshal::GetFunctionPointerForDelegate(errorcb);
	::vrpn_FORCEERRORHANDLER pErrorCallback = 
		static_cast<::vrpn_FORCEERRORHANDLER>(pError.ToPointer());

	if (m_force->register_force_change_handler(0, pForceCallback))
		throw gcnew VrpnException();
	
	if (m_force->register_scp_change_handler(0, pScpCallback))
		throw gcnew VrpnException();

	if (m_force->register_error_handler(0, pErrorCallback))
		throw gcnew VrpnException();
}

void ForceDeviceRemote::Update()
{
	m_force->mainloop();
}

Connection^ ForceDeviceRemote::GetConnection()
{
	return Connection::FromPointer(m_force->connectionPtr());
}

void ForceDeviceRemote::MuteWarnings::set(Boolean shutUp)
{
	m_force->shutup = shutUp;
}

Boolean ForceDeviceRemote::MuteWarnings::get()
{
	return m_force->shutup;
}

Boolean ForceDeviceRemote::SpringsAreForceFields::get()
{
#ifdef FD_SPRINGS_AS_FIELDS
	return true;
#else
	return false;
#endif
}

void ForceDeviceRemote::SendSurface()
{
	m_force->sendSurface();
}

void ForceDeviceRemote::StartSurface()
{
	m_force->startSurface();
}

void ForceDeviceRemote::StopSurface()
{
	m_force->stopSurface();
}

void ForceDeviceRemote::SetVertex(int vertexNumber, Vrpn::Vector3 point)
{
	m_force->setVertex(vertexNumber, 
		static_cast<float>(point.X), 
		static_cast<float>(point.Y),
		static_cast<float>(point.Z));
}

void ForceDeviceRemote::SetNormal(int normalNumber, Vrpn::Vector3 normal)
{
	m_force->setNormal(normalNumber, 
		static_cast<float>(normal.X), 
		static_cast<float>(normal.Y),
		static_cast<float>(normal.Z));
}

void ForceDeviceRemote::SetTriangle(int triangleNumber, int vertex1, int vertex2, int vertex3)
{
	SetTriangle(triangleNumber, vertex1, vertex2, vertex3, -1, -1, -1);
}

void ForceDeviceRemote::SetTriangle(int triangleNumber, int vertex1, int vertex2, int vertex3, int normal1, int normal2, int normal3)
{
	m_force->setTriangle(triangleNumber, vertex1, vertex2, vertex3,
		normal1, normal2, normal3);
}

void ForceDeviceRemote::RemoveTriangle(int triangleNumber)
{
	m_force->removeTriangle(triangleNumber);
}

void ForceDeviceRemote::UpdateTrimeshChanges()
{
	m_force->updateTrimeshChanges();
}

void ForceDeviceRemote::SetTrimeshTransform(cli::array<float> ^matrix)
{
	if (matrix->GetLength(0) != 16)
		throw gcnew System::ArgumentException("Value must be a 16-element homogenous transform matrix in row-major order.", "matrix");

	cli::pin_ptr<float> homMatrix = &matrix[0];

	m_force->setTrimeshTransform(static_cast<::vrpn_float32 *>(homMatrix));
}

void ForceDeviceRemote::ClearTrimesh()
{
	m_force->clearTrimesh();
}

void ForceDeviceRemote::AddObject(int objectNumber)
{
	AddObject(objectNumber, -1);
}

void ForceDeviceRemote::AddObject(int objectNumber, int parentNumber)
{
	m_force->addObject(objectNumber, parentNumber);
}

void ForceDeviceRemote::AddObjectExScene(int objectNumber)
{
	m_force->addObjectExScene(objectNumber);
}

void ForceDeviceRemote::SetObjectVertex(int objectNumber, int vertexNumber, Vrpn::Vector3 point)
{
	m_force->setObjectVertex(objectNumber, vertexNumber,
		static_cast<float>(point.X),
		static_cast<float>(point.Y),
		static_cast<float>(point.Z));
}

void ForceDeviceRemote::SetObjectNormal(int objectNumber, int normalNumber, Vrpn::Vector3 normal)
{
	m_force->setObjectNormal(objectNumber, normalNumber,
		static_cast<float>(normal.X),
		static_cast<float>(normal.Y),
		static_cast<float>(normal.Z));
}

void ForceDeviceRemote::SetObjectTriangle(int objectNumber, int triangleNumber, int vertex1, int vertex2, int vertex3)
{
	SetObjectTriangle(objectNumber, triangleNumber, vertex1, vertex2, vertex3,
		-1, -1, -1);
}

void ForceDeviceRemote::SetObjectTriangle(int objectNumber, int triangleNumber, int vertex1, int vertex2, int vertex3, int normal1, int normal2, int normal3)
{
	m_force->setObjectTriangle(objectNumber, triangleNumber, vertex1, vertex2, vertex3,
		normal1, normal2, normal3);
}

void ForceDeviceRemote::RemoveObjectTriangle(int objectNumber, int triangleNumber)
{
	m_force->removeObjectTriangle(objectNumber, triangleNumber);
}

void ForceDeviceRemote::UpdateObjectTrimeshChanges(int objectNumber)
{
	m_force->updateObjectTrimeshChanges(objectNumber);
}

void ForceDeviceRemote::SetObjectTrimeshTransform(int objectNumber, cli::array<float> ^matrix)
{
	if (matrix->GetLength(0) != 16)
		throw gcnew System::ArgumentException("Value must be a 16-element homogenous transform matrix in row-major order.", "matrix");

	cli::pin_ptr<float> homMatrix = &matrix[0];

	m_force->setObjectTrimeshTransform(objectNumber,
		static_cast<::vrpn_float32 *>(homMatrix));
}

void ForceDeviceRemote::SetObjectPosition(int objectNumber, Vrpn::Vector3 position)
{
	float cPos[3];
	VrpnUtils::CreateVector(position, cPos);

	m_force->setObjectPosition(objectNumber, cPos);
}

void ForceDeviceRemote::SetObjectOrientation(int objectNumber, Vrpn::Vector3 axis, float angle)
{
	float cAxis[3];
	VrpnUtils::CreateVector(axis, cAxis);

	m_force->setObjectOrientation(objectNumber, cAxis, angle);
}

void ForceDeviceRemote::SetObjectScale(int objectNumber, Vrpn::Vector3 scale)
{
	float cScale[3];
	VrpnUtils::CreateVector(scale, cScale);

	m_force->setObjectScale(objectNumber, cScale);
}

void ForceDeviceRemote::RemoveObject(int objectNumber)
{
	m_force->removeObject(objectNumber);
}

void ForceDeviceRemote::ClearObjectTrimesh(int objectNumber)
{
	m_force->clearObjectTrimesh(objectNumber);
}

void ForceDeviceRemote::MoveToParent(int objectNumber, int parentNumber)
{
	m_force->moveToParent(objectNumber, parentNumber);
}

void ForceDeviceRemote::SetHapticOrigin(Vrpn::Vector3 position, Vrpn::Vector3 axis, float angle)
{
	float cPos[3], cAxis[3];
	VrpnUtils::CreateVector(position, cPos);
	VrpnUtils::CreateVector(axis, cAxis);
	
	m_force->setHapticOrigin(cPos, cAxis, angle);
}

void ForceDeviceRemote::SetHapticScale(float scale)
{
	m_force->setHapticScale(scale);
}

void ForceDeviceRemote::SetSceneOrigin(Vrpn::Vector3 position, Vrpn::Vector3 axis, float angle)
{
	float cPos[3], cAxis[3];
	VrpnUtils::CreateVector(position, cPos);
	VrpnUtils::CreateVector(axis, cAxis);
	
	m_force->setSceneOrigin(cPos, cAxis, angle);
}

int ForceDeviceRemote::GetNewObjectId()
{
	return m_force->getNewObjectID();
}

void ForceDeviceRemote::SetObjectIsTouchable(int objectNumber, bool isTouchable)
{
	m_force->setObjectIsTouchable(objectNumber, isTouchable);
}

void ForceDeviceRemote::SetMeshPacking(Vrpn::ForceDeviceMeshPacking api)
{
	switch (api)
	{
	case ForceDeviceMeshPacking::Hcollide:
		m_force->useHcollide();
		break;

	case ForceDeviceMeshPacking::Ghost:
		m_force->useGhost();
		break;

	default:
		throw gcnew ArgumentOutOfRangeException("api");
	}
}

void ForceDeviceRemote::EnableConstraints(bool enable)
{
	m_force->enableConstraint(enable);
}

void ForceDeviceRemote::SetConstraintMode(ConstraintGeometry mode)
{
	m_force->setConstraintMode(static_cast<::vrpn_ForceDevice::ConstraintGeometry>(mode));
}

void ForceDeviceRemote::SetConstraintPoint(Vrpn::Vector3 point)
{
	float cPoint[3];
	VrpnUtils::CreateVector(point, cPoint);
	m_force->setConstraintPoint(cPoint);
}

void ForceDeviceRemote::SetConstraintLinePoint(Vrpn::Vector3 point)
{
	float cPoint[3];
	VrpnUtils::CreateVector(point, cPoint);
	m_force->setConstraintLinePoint(cPoint);
}

void ForceDeviceRemote::SetConstraintLineDirection(Vrpn::Vector3 direction)
{
	float cDir[3];
	VrpnUtils::CreateVector(direction, cDir);
	m_force->setConstraintLineDirection(cDir);
}

void ForceDeviceRemote::SetConstraintPlanePoint(Vrpn::Vector3 point)
{
	float cPoint[3];
	VrpnUtils::CreateVector(point, cPoint);
	m_force->setConstraintPlanePoint(cPoint);
}

void ForceDeviceRemote::SetConstraintPlaneNormal(Vrpn::Vector3 normal)
{
	float cNormal[3];
	VrpnUtils::CreateVector(normal, cNormal);
	m_force->setConstraintPlaneNormal(cNormal);
}

void ForceDeviceRemote::SetConstraintKSpring(float k)
{
	m_force->setConstraintKSpring(k);
}

void ForceDeviceRemote::SendForceField()
{
	m_force->sendForceField();
}

void ForceDeviceRemote::SendForceField(Vrpn::Vector3 origin, Vrpn::Vector3 force, cli::array<float,2> ^jacobian, float radius)
{
	if (jacobian->GetLength(0) != 3 || jacobian->GetLength(1) != 3)
		throw gcnew ArgumentException("jacobian", "Value must be a 3*3 matrix.");

	float cOrigin[3], cForce[3];
	VrpnUtils::CreateVector(origin, cOrigin);
	VrpnUtils::CreateVector(force, cForce);

	float cJacobian[3][3];
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			cJacobian[i][j] = jacobian[i, j];

	m_force->sendForceField(cOrigin, cForce, cJacobian, radius);
}

void ForceDeviceRemote::StopForceField()
{
	m_force->stopForceField();
}

void ForceDeviceRemote::StartEffect()
{
	m_force->startEffect();
}

void ForceDeviceRemote::StopEffect()
{
	m_force->stopEffect();
}

void ForceDeviceRemote::onSurfaceContact(void *, const vrpn_FORCESCPCB info)
{
	ForceSurfaceContactEventArgs ^e = gcnew ForceSurfaceContactEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Position = VrpnUtils::ConvertVector(info.pos);
	e->Orientation = VrpnUtils::ConvertQuat(info.quat);

	SurfaceContact(this, e);
}

void ForceDeviceRemote::onForceChange(void *, const vrpn_FORCECB info)
{
	ForceChangeEventArgs ^e = gcnew ForceChangeEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Force = VrpnUtils::ConvertVector(info.force);
	ForceChange(this, e);
}

void ForceDeviceRemote::onErrorReport(void *, const vrpn_FORCEERRORCB info)
{
	ForceErrorEventArgs ^e = gcnew ForceErrorEventArgs();
	e->Time = VrpnUtils::ConvertTimeval(info.msg_time);
	e->Error = safe_cast<ForceDeviceError>(info.error_code);
	Error(this, e);
}