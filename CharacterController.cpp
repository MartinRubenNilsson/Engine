#include "pch.h"
#include "CharacterController.h"
#include "PhysX.h"

#define DEFAULT_RADIUS 0.5f
#define DEFAULT_HEIGHT 2.f

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
	desc.material = PhysX::Get().GetDefaultMaterial();
	desc.radius = DEFAULT_RADIUS;
	desc.height = DEFAULT_HEIGHT;
	if (!desc.isValid())
		return;

	PxController* controller = PhysX::Get().GetControllerMgr()->createController(desc);
	myImpl.reset(static_cast<PxCapsuleController*>(controller));
}

CharacterController::CharacterController(const CharacterController& other)
	: CharacterController()
{
	Copy(myImpl.get(), other.myImpl.get());
	myMinMoveDistance = other.myMinMoveDistance;
}

CharacterController& CharacterController::operator=(const CharacterController& other)
{
	Copy(myImpl.get(), other.myImpl.get());
	myMinMoveDistance = other.myMinMoveDistance;
	return *this;
}

CollisionFlags CharacterController::Move(const Vector3& aDeltaPos, float aDeltaTime)
{
	CollisionFlags flags{};
	if (myImpl)
	{
		PxControllerCollisionFlags pxFlags = myImpl->move(ToPx(aDeltaPos), myMinMoveDistance, aDeltaTime, {});
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
	return myImpl ? FromPx(myImpl->getPosition()) : Vector3{};
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

CharacterController::operator bool() const
{
	return myImpl.operator bool();
}

void from_json(const json& j, CharacterController& c)
{
	auto pos = j.at("position").get<std::array<float, 3>>();

	c.SetPosition(Vector3{ pos.data() });
	c.SetRadius(j.at("radius"));
	c.SetHeight(j.at("height"));
	j.at("minMoveDistance").get_to(c.myMinMoveDistance);
}

void to_json(json& j, const CharacterController& c)
{
	auto pos = c.GetPosition();

	j["position"] = { pos.x, pos.y, pos.z };
	j["radius"] = c.GetRadius();
	j["height"] = c.GetHeight();
	j["minMoveDistance"] = c.myMinMoveDistance;
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
}
