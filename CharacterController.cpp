#include "pch.h"
#include "CharacterController.h"
#include "PhysX.h"

#define DEFAULT_RADIUS 0.5f
#define DEFAULT_HEIGHT 2.f

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