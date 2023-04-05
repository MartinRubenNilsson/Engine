#pragma once

PxVec3 ToPx(const Vector3&);
Vector3 FromPx(const PxVec3&);

PxQuat ToPx(const Quaternion&);
Quaternion FromPx(const PxQuat&);

PxExtendedVec3 ToPxEx(const Vector3&);
Vector3 FromPxEx(const PxExtendedVec3&);

namespace PhysX
{
	bool Create();
	void Destroy();
	PxPhysics* GetPhysics();
	PxScene* GetScene();
	PxControllerManager* GetControllerMgr();
	PxMaterial* GetDefaultMaterial();

	struct Scope
	{
		const bool ok;

		Scope() : ok{ Create() } {}
		~Scope() { Destroy(); }
	};
}

namespace ImGui
{
	void PhysX();
}
