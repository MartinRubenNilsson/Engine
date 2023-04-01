#include "pch.h"
#include "EnttCommon.h"
#include "Tags.h"

// Components
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"

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
	return Transform::Create(aRegistry).GetEntity();
}

entt::entity CreateEmptyWithMaterial(entt::registry& aRegistry)
{
	entt::entity entity = CreateEmpty(aRegistry);
	aRegistry.emplace<Material>(entity);
	return entity;
}

entt::entity CreatePlane(entt::registry& aRegistry)
{
	entt::entity entity = CreateEmptyWithMaterial(aRegistry);
	aRegistry.emplace<Mesh>(entity, MeshPrimitiveType::Plane);
	return entity;
}

entt::entity CreateCube(entt::registry& aRegistry)
{
	entt::entity entity = CreateEmptyWithMaterial(aRegistry);
	aRegistry.emplace<Mesh>(entity, MeshPrimitiveType::Cube);
	return entity;
}

entt::entity CreateSphere(entt::registry& aRegistry)
{
	entt::entity entity = CreateEmptyWithMaterial(aRegistry);
	aRegistry.emplace<Mesh>(entity, MeshPrimitiveType::Sphere);
	return entity;
}

entt::entity CreateCylinder(entt::registry& aRegistry)
{
	entt::entity entity = CreateEmptyWithMaterial(aRegistry);
	aRegistry.emplace<Mesh>(entity, MeshPrimitiveType::Cylinder);
	return entity;
}

entt::entity CreateCone(entt::registry& aRegistry)
{
	entt::entity entity = CreateEmptyWithMaterial(aRegistry);
	aRegistry.emplace<Mesh>(entity, MeshPrimitiveType::Cone);
	return entity;
}

entt::entity CreateTorus(entt::registry& aRegistry)
{
	entt::entity entity = CreateEmptyWithMaterial(aRegistry);
	aRegistry.emplace<Mesh>(entity, MeshPrimitiveType::Torus);
	return entity;
}

entt::entity CreateSuzanne(entt::registry& aRegistry)
{
	entt::entity entity = CreateEmptyWithMaterial(aRegistry);
	aRegistry.emplace<Mesh>(entity, MeshPrimitiveType::Suzanne);
	return entity;
}
