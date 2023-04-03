#include "pch.h"
#include "Systems.h"
#include "PhysicsSystems.h"

void Systems::Update(entt::registry& aRegistry)
{
	UpdateTransformsOfRigidbodies(aRegistry);
	MoveCharacterControllersUsingKeyboard(aRegistry);
}
