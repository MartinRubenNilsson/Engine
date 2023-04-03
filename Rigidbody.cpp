#include "pch.h"
#include "Rigidbody.h"
#include "PhysX.h"

Rigidbody::Rigidbody()
{
	PxTransform pose{ PxIdentity };
	PxRigidDynamic* rigid = PhysX::GetPhysics()->createRigidDynamic(pose);
	myImpl.reset(rigid);
	PhysX::GetScene()->addActor(*rigid);
}

Rigidbody::Rigidbody(const Rigidbody& other)
{
	if (other)
	{
		PxRigidDynamic* rigid = PxCloneDynamic(*PhysX::GetPhysics(), other.myImpl->getGlobalPose(), *other.myImpl);
		myImpl.reset(rigid);
		PhysX::GetScene()->addActor(*rigid);
	}
}

Rigidbody& Rigidbody::operator=(const Rigidbody& other)
{
	if (other)
	{
		PxRigidDynamic* rigid = PxCloneDynamic(*PhysX::GetPhysics(), other.myImpl->getGlobalPose(), *other.myImpl);
		myImpl.reset(rigid);
		PhysX::GetScene()->addActor(*rigid);
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
