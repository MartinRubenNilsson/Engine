#include "pch.h"
#include "EnttCommon.h"
#include "Tags.h"

// Components
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "CharacterController.h"

float GetDeltaTime(const entt::registry& aRegistry)
{
	return aRegistry.ctx().get<float>("deltaTime"_hs);
}

/*
* Misc
*/

entt::entity Find(const entt::registry& aRegistry, std::string_view aName)
{
	for (auto [entity, name] : aRegistry.view<std::string>().each())
	{
		if (name == aName)
			return entity;
	}
	return entt::null;
}

/*
* Selection
*/


void Select(entt::registry& aRegistry, entt::entity anEntity)
{
	auto selection = aRegistry.view<Tag::Selected>();
	aRegistry.erase<Tag::Selected>(selection.begin(), selection.end());

	if (aRegistry.valid(anEntity))
		aRegistry.emplace_or_replace<Tag::Selected>(anEntity);
}

entt::entity GetSelected(entt::registry& aRegistry)
{
	return aRegistry.view<Tag::Selected>().front();
}

/*
* Creation
*/

entt::entity CreateEmpty(entt::registry& aRegistry)
{
	entt::entity entity = Transform::Create(aRegistry).GetEntity();
	aRegistry.emplace<std::string>(entity, "Entity");
	return entity;
}

entt::entity CreatePlane(entt::registry& aRegistry)
{
	entt::entity entity = Transform::Create(aRegistry).GetEntity();
	aRegistry.emplace<std::string>(entity, "Plane");
	aRegistry.emplace<Mesh>(entity, MeshPrimitiveType::Plane);
	aRegistry.emplace<Material>(entity);
	return entity;
}

entt::entity CreateCube(entt::registry& aRegistry)
{
	entt::entity entity = Transform::Create(aRegistry).GetEntity();
	aRegistry.emplace<std::string>(entity, "Cube");
	aRegistry.emplace<Mesh>(entity, MeshPrimitiveType::Cube);
	aRegistry.emplace<Material>(entity);
	return entity;
}

entt::entity CreateSphere(entt::registry& aRegistry)
{
	entt::entity entity = Transform::Create(aRegistry).GetEntity();
	aRegistry.emplace<std::string>(entity, "Sphere");
	aRegistry.emplace<Mesh>(entity, MeshPrimitiveType::Sphere);
	aRegistry.emplace<Material>(entity);
	return entity;
}

entt::entity CreateCylinder(entt::registry& aRegistry)
{
	entt::entity entity = Transform::Create(aRegistry).GetEntity();
	aRegistry.emplace<std::string>(entity, "Cylinder");
	aRegistry.emplace<Mesh>(entity, MeshPrimitiveType::Cylinder);
	aRegistry.emplace<Material>(entity);
	return entity;
}

entt::entity CreateCone(entt::registry& aRegistry)
{
	entt::entity entity = Transform::Create(aRegistry).GetEntity();
	aRegistry.emplace<std::string>(entity, "Cone");
	aRegistry.emplace<Mesh>(entity, MeshPrimitiveType::Cone);
	aRegistry.emplace<Material>(entity);
	return entity;
}

entt::entity CreateTorus(entt::registry& aRegistry)
{
	entt::entity entity = Transform::Create(aRegistry).GetEntity();
	aRegistry.emplace<std::string>(entity, "Torus");
	aRegistry.emplace<Mesh>(entity, MeshPrimitiveType::Torus);
	aRegistry.emplace<Material>(entity);
	return entity;
}

entt::entity CreateSuzanne(entt::registry& aRegistry)
{
	entt::entity entity = Transform::Create(aRegistry).GetEntity();
	aRegistry.emplace<std::string>(entity, "Suzanne");
	aRegistry.emplace<Mesh>(entity, MeshPrimitiveType::Suzanne);
	aRegistry.emplace<Material>(entity);
	return entity;
}

/*
* Manipulation
*/

//void SetWorldPosition(entt::registry& aRegistry, entt::entity anEntity, const Vector3& aPos)
//{
//	if (auto transform = aRegistry.try_get<Transform>(anEntity))
//		transform->SetWorldPosition(aRegistry, aPos);
//	if (auto controller = aRegistry.try_get<CharacterController>(anEntity))
//		controller->SetPosition(aPos);
//}

void SetWorldMatrix(entt::registry& aRegistry, entt::entity anEntity, const Matrix& aMatrix)
{
	if (auto transform = aRegistry.try_get<Transform>(anEntity))
		transform->SetWorldMatrix(aRegistry, aMatrix);
	if (auto controller = aRegistry.try_get<CharacterController>(anEntity))
		controller->SetPosition(aMatrix.Translation());
}

void Move(entt::registry& aRegistry, entt::entity anEntity, const Vector3& aDeltaPos)
{
	if (!aRegistry.all_of<CharacterController, Transform>(anEntity))
		return;

	auto& controller = aRegistry.get<CharacterController>(anEntity);
	auto& transform = aRegistry.get<Transform>(anEntity);

	controller.Move(aDeltaPos, GetDeltaTime(aRegistry));
	transform.SetWorldPosition(aRegistry, controller.GetPosition());
}
