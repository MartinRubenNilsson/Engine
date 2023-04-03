#include "pch.h"
#include "CharacterController.h"
#include "PhysX.h"
#include "SimpleMathSerialization.h"

namespace
{
	void Copy(PxCapsuleController* dst, const PxCapsuleController* src)
	{
		if (!dst || !src || dst == src)
			return;

		dst->setPosition(src->getPosition());
		dst->setRadius(src->getRadius());
		dst->setHeight(src->getHeight());
	}
}

CharacterController::CharacterController()
{
	PxCapsuleControllerDesc desc{};
	desc.material = PhysX::GetDefaultMaterial();
	desc.radius = 0.5f;
	desc.height = 2.f;
	if (!desc.isValid())
		return;

	PxController* controller = PhysX::GetControllerMgr()->createController(desc);
	myImpl.reset(static_cast<PxCapsuleController*>(controller));
}

CharacterController::CharacterController(const CharacterController& other)
	: CharacterController()
{
	Copy(myImpl.get(), other.myImpl.get());
	minMoveDistance = other.minMoveDistance;
}

CharacterController& CharacterController::operator=(const CharacterController& other)
{
	Copy(myImpl.get(), other.myImpl.get());
	minMoveDistance = other.minMoveDistance;
	return *this;
}

CollisionFlags CharacterController::Move(const Vector3& aDeltaPos, float aDeltaTime)
{
	CollisionFlags flags{};

	if (myImpl)
	{
		PxControllerCollisionFlags pxFlags = myImpl->move(ToPx(aDeltaPos), minMoveDistance, aDeltaTime, {});
		flags = CollisionFlags{ pxFlags.operator uint8_t() };
	}

	return flags;
}

void CharacterController::SetPosition(const Vector3& pos)
{
	if (myImpl)
		myImpl->setPosition(ToPxEx(pos));
}

Vector3 CharacterController::GetPosition() const
{
	return myImpl ? FromPxEx(myImpl->getPosition()) : Vector3{};
}

void CharacterController::SetRadius(float aRadius)
{
	if (myImpl)
		myImpl->setRadius(aRadius);
}

float CharacterController::GetRadius() const
{
	return myImpl ? myImpl->getRadius() : 0.f;
}

void CharacterController::SetHeight(float aHeight)
{
	if (myImpl)
		myImpl->setHeight(aHeight);
}

float CharacterController::GetHeight() const
{
	return myImpl ? myImpl->getHeight() : 0.f;
}

PxCapsuleController* CharacterController::GetImpl()
{
	return myImpl.get();
}

CharacterController::operator bool() const
{
	return myImpl.operator bool();
}

void from_json(const json& j, CharacterController& c)
{
	c.SetPosition(j.at("position"));
	c.SetRadius(j.at("radius"));
	c.SetHeight(j.at("height"));
	j.at("minMoveDistance").get_to(c.minMoveDistance);
}

void to_json(json& j, const CharacterController& c)
{
	j["position"] = c.GetPosition();
	j["radius"] = c.GetRadius();
	j["height"] = c.GetHeight();
	j["minMoveDistance"] = c.minMoveDistance;
}

/*
* namespace ImGui
*/

void ImGui::Inspect(CharacterController& c)
{
	float radius = c.GetRadius();
	float height = c.GetHeight();

	if (DragFloat("Radius", &radius, 0.01f, 0.01f, FLT_MAX))
		c.SetRadius(radius);
	if (DragFloat("Height", &height, 0.01f, 0.01f, FLT_MAX))
		c.SetHeight(height);
	DragFloat("Min Move Distance", &c.minMoveDistance, 0.01f, 0.f, FLT_MAX);
}
