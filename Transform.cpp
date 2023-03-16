#include "pch.h"
#include "Transform.h"

/*
* class Transform
*/

Transform& Transform::CreateHierarchy(entt::registry& aRegistry, aiNode* aNode)
{
	entt::entity entity = aRegistry.create();

	Transform& transform = aRegistry.emplace<Transform>(entity);
	transform.myEntity = entity;
	transform.myName = aNode->mName.C_Str();
	std::memcpy(&transform.myLocalMatrix, &aNode->mTransformation.Transpose(), sizeof(Matrix));

	aRegistry.emplace<aiNode*>(entity, aNode);

	for (aiNode* childNode : std::span{ aNode->mChildren, aNode->mNumChildren })
	{
		Transform& childTransform = CreateHierarchy(aRegistry, childNode);
		transform.myChildren.push_back(childTransform.myEntity);
		childTransform.myParent = entity;
	}

	return transform;
}

entt::entity Transform::Find(const entt::registry& aRegistry, std::string_view aName) const
{
	if (aName == myName)
		return myEntity;

	for (entt::entity child : myChildren)
	{
		if (auto transform = aRegistry.try_get<Transform>(child))
		{
			entt::entity entity = transform->Find(aRegistry, aName);
			if (aRegistry.valid(entity))
				return entity;
		}
	}

	return entt::null;
}

void Transform::Destroy(entt::registry& aRegistry)
{
	entt::entity entity = myEntity;
	entt::entity parent = myParent;
	auto children{ myChildren };

	if (aRegistry.valid(entity))
		aRegistry.destroy(entity);

	for (entt::entity child : children)
		aRegistry.get<Transform>(child).Destroy(aRegistry);

	if (auto t = aRegistry.try_get<Transform>(parent))
		t->RemoveChild(entity);
}

size_t Transform::GetDepth(const entt::registry& aRegistry) const
{
	size_t depth = 0;
	if (auto transform = aRegistry.try_get<Transform>(myParent))
		depth = 1 + transform->GetDepth(aRegistry);
	return depth;
}

void Transform::SetWorldMatrix(const entt::registry& aRegistry, const Matrix& aMatrix)
{
	myLocalMatrix = aMatrix;
	if (auto transform = aRegistry.try_get<Transform>(myParent))
		myLocalMatrix *= transform->GetWorldMatrix(aRegistry).Invert();
}

Matrix Transform::GetWorldMatrix(const entt::registry& aRegistry) const
{
	Matrix result = myLocalMatrix;
	if (auto transform = aRegistry.try_get<Transform>(myParent))
		result *= transform->GetWorldMatrix(aRegistry);
	return result;
}

void Transform::SetParent(entt::registry& aRegistry, entt::entity anEntity, bool aWorldTransformStays)
{
	if (myParent == anEntity)
		return;

	if (auto transform = aRegistry.try_get<Transform>(anEntity))
	{
		if (transform->IsChildOf(aRegistry, myEntity))
			return;
		transform->AddChild(myEntity);
	}

	if (auto parent = aRegistry.try_get<Transform>(myParent))
		parent->RemoveChild(myEntity);

	if (aWorldTransformStays)
	{
		Matrix worldMatrix = GetWorldMatrix(aRegistry);
		myParent = anEntity;
		SetWorldMatrix(aRegistry, worldMatrix);
	}
	else
	{
		myParent = anEntity;
	}
}

bool Transform::IsChildOf(entt::registry& aRegistry, entt::entity anEntity) const
{
	for (auto t = this; t; t = aRegistry.try_get<Transform>(t->myParent))
	{
		if (t->myEntity == anEntity)
			return true;
	}

	return false;
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

float* Transform::Data()
{
	return &myLocalMatrix._11;
}

const float* Transform::Data() const
{
	return &myLocalMatrix._11;
}

void from_json(const json& j, Transform& t)
{
	std::array<float, 16> matrix{};

	j.at("entity").get_to(t.myEntity);
	j.at("parent").get_to(t.myParent);
	j.at("children").get_to(t.myChildren);
	j.at("name").get_to(t.myName);
	j.at("matrix").get_to(matrix);

	std::memcpy(t.Data(), matrix.data(), sizeof(Matrix));
}

void to_json(json& j, const Transform& t)
{
	std::array<float, 16> matrix{};
	std::memcpy(matrix.data(), t.Data(), sizeof(Matrix));

	j["entity"] = t.myEntity;
	j["parent"] = t.myParent;
	j["children"] = t.myChildren;
	j["name"] = t.myName;
	j["matrix"] = matrix;
}

/*
* namespace ImGui
*/

void ImGui::Inspect(Transform& aTransform)
{
	Value("Entity", std::to_underlying(aTransform.GetEntity()));

	std::string name{ aTransform.GetName() };
	if (InputText("Name", &name))
		aTransform.SetName(name);

	float translation[3]{};
	float rotation[3]{};
	float scale[3]{};

	ImGuizmo::DecomposeMatrixToComponents(aTransform.Data(), translation, rotation, scale);
	DragFloat3("Translation", translation, 0.025f);
	DragFloat3("Rotation", rotation, 0.25f);
	DragFloat3("Scale", scale, 0.025f);
	ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, aTransform.Data());
}
