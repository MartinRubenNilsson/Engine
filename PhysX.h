#pragma once

PxVec3 ToPx(const Vector3&);
PxExtendedVec3 ToPxEx(const Vector3&);
Vector3 FromPx(const PxExtendedVec3&);

namespace PhysX
{
	bool Create();

	PxPhysics* GetPhysics();
	PxScene* GetScene();
	PxControllerManager* GetControllerMgr();
	PxMaterial* GetDefaultMaterial();
}

namespace ImGui
{
	void PhysX();
}
