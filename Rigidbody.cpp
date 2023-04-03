#include "pch.h"
#include "Rigidbody.h"
#include "PhysX.h"

Rigidbody::Rigidbody()
{
	PxTransform pose{ PxIdentity };
	PxRigidDynamic* rigid = PX_PHYSICS->createRigidDynamic(pose);
	myImpl.reset(rigid);
	PX_SCENE->addActor(*rigid);
}

Rigidbody::Rigidbody(const Rigidbody& other)
{
	if (other)
	{
		PxRigidDynamic* rigid = PxCloneDynamic(*PX_PHYSICS, other.myImpl->getGlobalPose(), *other.myImpl);
		myImpl.reset(rigid);
		PX_SCENE->addActor(*rigid);
	}
}

Rigidbody& Rigidbody::operator=(const Rigidbody& other)
{
	if (other)
	{
		PxRigidDynamic* rigid = PxCloneDynamic(*PX_PHYSICS, other.myImpl->getGlobalPose(), *other.myImpl);
		myImpl.reset(rigid);
		PX_SCENE->addActor(*rigid);
	}
	return *this;
}

Rigidbody::operator bool() const
{
	return myImpl.operator bool();
}

void from_json(const json&, Rigidbody&)
{
	// todo
}

void to_json(json&, const Rigidbody&)
{
	 // todo
}

void ImGui::Inspect(Rigidbody&)
{
	// todo
}
