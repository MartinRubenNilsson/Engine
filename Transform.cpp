#include "pch.h"
#include "Transform.h"
#include "SimpleMathSerialization.h"

/*
* class Transform
*/

Transform& Transform::Create(entt::registry& aRegistry)
{
	entt::entity entity = aRegistry.create();
	Transform& transform = aRegistry.emplace<Transform>(entity);
	transform.myEntity = entity;

	return transform;
}

Transform& Transform::CreateChild(entt::registry& aRegistry)
{
	Transform& child = Create(aRegistry);
	myChildren.push_back(child.myEntity);
	child.myParent = myEntity;

	return child;
}

Transform& Transform::Duplicate(entt::registry& aRegistry, entt::entity aParent, bool aWorldTransformStays)
{
	Transform& copy = Create(aRegistry);
	copy.myParent = aParent;

	if (aWorldTransformStays)
		copy.SetWorldMatrix(aRegistry, GetWorldMatrix(aRegistry));
	else
		copy.SetMatrix(GetMatrix());

	if (auto parent = aRegistry.try_get<Transform>(aParent))
		parent->AddChild(copy.myEntity);

	for (auto [_, storage] : aRegistry.storage())
	{
		if (storage.type() != entt::type_id<Transform>() && storage.contains(myEntity))
			storage.emplace(copy.myEntity, storage.get(myEntity));
	}

	for (entt::entity child : myChildren)
		aRegistry.get<Transform>(child).Duplicate(aRegistry, copy.myEntity, false);

	return copy;
}

void Transform::Destroy(entt::registry& aRegistry)
{
	entt::entity me = myEntity;
	entt::entity parent = myParent;
	auto children{ myChildren };

	if (aRegistry.valid(me))
		aRegistry.destroy(me);

	for (entt::entity child : children)
	{
		if (auto transform = aRegistry.try_get<Transform>(child))
			transform->Destroy(aRegistry);
	}

	if (auto transform = aRegistry.try_get<Transform>(parent))
		transform->RemoveChild(me);
}

size_t Transform::GetDepth(const entt::registry& aRegistry) const
{
	size_t depth = 0;
	if (auto transform = aRegistry.try_get<Transform>(myParent))
		depth = 1 + transform->GetDepth(aRegistry);
	return depth;
}

void Transform::SetMatrix(Matrix aMatrix)
{
	aMatrix.Decompose(scale, rotation, position);
}

Matrix Transform::GetMatrix() const
{
	return Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(position);
}

void Transform::SetWorldMatrix(const entt::registry& aRegistry, Matrix aMatrix)
{
	if (auto transform = aRegistry.try_get<Transform>(myParent))
		aMatrix *= transform->GetWorldMatrix(aRegistry).Invert();
	SetMatrix(aMatrix);
}

Matrix Transform::GetWorldMatrix(const entt::registry& aRegistry) const
{
	Matrix result = GetMatrix();
	if (auto transform = aRegistry.try_get<Transform>(myParent))
		result *= transform->GetWorldMatrix(aRegistry);
	return result;
}

void Transform::SetWorldPosition(const entt::registry& aRegistry, const Vector3& aPosition)
{
	Matrix matrix = GetWorldMatrix(aRegistry);
	matrix._41 = aPosition.x;
	matrix._42 = aPosition.y;
	matrix._43 = aPosition.z;
	SetWorldMatrix(aRegistry, matrix);
}

void Transform::SetParent(entt::registry& aRegistry, entt::entity aParent, bool aWorldTransformStays)
{
	if (myParent == aParent)
		return;

	if (auto newParent = aRegistry.try_get<Transform>(aParent))
	{
		if (newParent->IsChildOf(aRegistry, myEntity))
			return;
		newParent->AddChild(myEntity);
	}

	if (auto oldParent = aRegistry.try_get<Transform>(myParent))
		oldParent->RemoveChild(myEntity);

	if (aWorldTransformStays)
	{
		Matrix worldMatrix = GetWorldMatrix(aRegistry);
		myParent = aParent;
		SetWorldMatrix(aRegistry, worldMatrix);
	}
	else
	{
		myParent = aParent;
	}
}

bool Transform::IsChildOf(const entt::registry& aRegistry, entt::entity anEntity) const
{
	for (auto t = this; t; t = aRegistry.try_get<Transform>(t->myParent))
	{
		if (t->myEntity == anEntity)
			return true;
	}

	return false;
}

bool Transform::HasParent(const entt::registry& aRegistry) const
{
	return aRegistry.valid(myParent);
}

void Transform::AddChild(entt::entity anEntity)
{
	if (!std::ranges::contains(myChildren, anEntity))
		myChildren.push_back(anEntity);
}

void Transform::RemoveChild(entt::entity anEntity)
{
	auto range = std::ranges::remove(myChildren, anEntity);
	myChildren.erase(range.begin(), range.end());
}

void from_json(const json& j, Transform& t)
{
	j.at("position").get_to(t.position);
	j.at("rotation").get_to(t.rotation);
	j.at("scale").get_to(t.scale);
	j.at("entity").get_to(t.myEntity);
	j.at("parent").get_to(t.myParent);
	j.at("children").get_to(t.myChildren);
}

void to_json(json& j, const Transform& t)
{
	j["position"] = t.position;
	j["rotation"] = t.rotation;
	j["scale"] = t.scale;
	j["entity"] = t.myEntity;
	j["parent"] = t.myParent;
	j["children"] = t.myChildren;
}

/*
* namespace ImGui
*/

void ImGui::Inspect(Transform& t)
{
	DragFloat3("Position", &t.position.x, 0.025f);

	// Todo: use imguizmo decompose instead
	Vector3 euler = t.rotation.ToEuler();
	if (DragFloat3("Rotation", &euler.x, 0.25f))
		t.rotation = Quaternion::CreateFromYawPitchRoll(euler);

	DragFloat3("Scale", &t.scale.x, 0.025f);
}
