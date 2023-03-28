#include "pch.h"
#include "PhysicMaterial.h"
#include "PhysX.h"

PhysicMaterial::PhysicMaterial()
{
	myImpl.reset(PX_PHYSICS->createMaterial(0.5f, 0.5f, 0.6f));
}

void PhysicMaterial::SetEntity(entt::entity anEntity)
{
	if (myImpl)
		myImpl->userData = reinterpret_cast<void*>(anEntity);
}

entt::entity PhysicMaterial::GetEntity() const
{
	if (myImpl)
	{
		auto ptr = reinterpret_cast<uintptr_t>(myImpl->userData);
		return static_cast<entt::entity>(ptr);
	}
	return entt::null;
}

PhysicMaterial::operator bool() const
{
	return myImpl.operator bool();
}
