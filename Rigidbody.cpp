#include "pch.h"
#include "Rigidbody.h"
#include "PhysX.h"
#include "SimpleMathSerialization.h"

Rigidbody::Rigidbody()
{
	PxTransform transform{ PxIdentity };
	PxRigidDynamic* rigid = PhysX::GetPhysics()->createRigidDynamic(transform);
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

void Rigidbody::SetTransform(const Vector3& p, const Quaternion& q)
{
	if (myImpl)
		myImpl->setGlobalPose({ ToPx(p), ToPx(q) });
}

void Rigidbody::GetTransform(Vector3& p, Quaternion& q) const
{
	if (myImpl)
	{
		PxTransform transform{ myImpl->getGlobalPose() };
		p = FromPx(transform.p);
		q = FromPx(transform.q);
	}
}

Vector3 Rigidbody::GetVelocity() const
{
	return myImpl ? FromPx(myImpl->getLinearVelocity()) : Vector3{};
}

Rigidbody::operator bool() const
{
	return myImpl.operator bool();
}

void from_json(const json& j, Rigidbody& r)
{
	r.SetTransform(j.at("position"), j.at("rotation"));
}

void to_json(json& j, const Rigidbody& r)
{
	Vector3 p{};
	Quaternion q{};
	r.GetTransform(p, q);

	j["position"] = p;
	j["rotation"] = q;
}

/*
* namespace ImGui
*/

void ImGui::Inspect(Rigidbody& r)
{
	if (TreeNodeEx("Info", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Vector3 vel = r.GetVelocity();
		float speed = vel.Length();

		BeginDisabled();
		DragFloat("Speed", &speed);
		DragFloat3("Velocity", &vel.x);
		EndDisabled();
		TreePop();
	}
}
