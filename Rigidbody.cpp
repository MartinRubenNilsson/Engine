#include "pch.h"
#include "Rigidbody.h"
#include "PhysX.h"
#include "SimpleMathSerialization.h"

#define MIN_MASS 0.00001f

Rigidbody::Rigidbody()
{
	PxTransform transform{ PxIdentity };
	PxRigidDynamic* rigid = PhysX::GetPhysics()->createRigidDynamic(transform);
	myImpl.reset(rigid);
	if (rigid)
		PhysX::GetScene()->addActor(*rigid);
}

Rigidbody::Rigidbody(const Rigidbody& other)
{
	if (other)
	{
		PxRigidDynamic* rigid = PxCloneDynamic(*PhysX::GetPhysics(), other.myImpl->getGlobalPose(), *other.myImpl);
		myImpl.reset(rigid);
		if (rigid)
		{
			myImpl->setLinearDamping(other.myImpl->getLinearDamping()); // Should be copied by PxCloneDynamic but isn't
			PhysX::GetScene()->addActor(*rigid);
		}
	}
}

Rigidbody& Rigidbody::operator=(const Rigidbody& other)
{
	if (other)
	{
		PxRigidDynamic* rigid = PxCloneDynamic(*PhysX::GetPhysics(), other.myImpl->getGlobalPose(), *other.myImpl);
		myImpl.reset(rigid);
		if (rigid)
		{
			myImpl->setLinearDamping(other.myImpl->getLinearDamping()); // Should be copied by PxCloneDynamic but isn't
			PhysX::GetScene()->addActor(*rigid);
		}
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

void Rigidbody::SetMass(float aMass)
{
	if (myImpl)
		PxRigidBodyExt::setMassAndUpdateInertia(*myImpl, aMass);
}

float Rigidbody::GetMass() const
{
	return myImpl ? myImpl->getMass() : MIN_MASS;
}

void Rigidbody::SetDrag(float aDrag)
{
	if (myImpl)
		myImpl->setLinearDamping(aDrag);
}

float Rigidbody::GetDrag() const
{
	return myImpl ? myImpl->getLinearDamping() : 0.f;
}

bool Rigidbody::IsSleeping() const
{
	return myImpl ? myImpl->isSleeping() : true;
}

Rigidbody::operator bool() const
{
	return myImpl.operator bool();
}

void from_json(const json& j, Rigidbody& r)
{
	r.SetTransform(j.at("position"), j.at("rotation"));
	r.SetMass(j.at("mass"));
	r.SetDrag(j.at("drag"));
}

void to_json(json& j, const Rigidbody& r)
{
	Vector3 p{};
	Quaternion q{};
	r.GetTransform(p, q);

	j["position"] = p;
	j["rotation"] = q;
	j["mass"] = r.GetMass();
	j["drag"] = r.GetDrag();
}

/*
* namespace ImGui
*/

void ImGui::Inspect(Rigidbody& r)
{
	float mass = r.GetMass();
	float drag = r.GetDrag();

	if (DragFloat("Mass", &mass, 0.1f, MIN_MASS, FLT_MAX))
		r.SetMass(mass);
	if (DragFloat("Drag", &drag, 0.01f, 0.f, FLT_MAX))
		r.SetDrag(drag);

	// todo: drag is always set to 0.5 when reloading scene

	if (TreeNodeEx("Info", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Vector3 vel = r.GetVelocity();
		float speed = vel.Length();

		BeginDisabled();
		DragFloat("Speed", &speed);
		DragFloat3("Velocity", &vel.x);
		Text("Sleep State: %s", r.IsSleeping() ? "Sleeping" : "Awake");
		EndDisabled();
		TreePop();
	}
}
