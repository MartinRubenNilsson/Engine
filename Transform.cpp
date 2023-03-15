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

//void Transform::SetParent(Ptr aParent, bool aWorldPositionStays)
//{
//	auto me = shared_from_this();
//	if (aParent)
//	{
//		if (aParent->IsChildOf(me))
//			return;
//		aParent->myChildren.push_back(me);
//	}
//	if (myParent)
//	{
//		auto& siblings = myParent->myChildren;
//		siblings.erase(std::remove(siblings.begin(), siblings.end(), me));
//	}
//	if (aWorldPositionStays)
//	{
//		Matrix worldMatrix = GetWorldMatrix();
//		myParent = aParent.get();
//		SetWorldMatrix(worldMatrix);
//	}
//	else
//	{
//		myParent = aParent.get();
//	}
//}

//size_t Transform::GetTreeSize() const
//{
//	size_t size = 1;
//	for (const auto& child : myChildren)
//		size += child->GetTreeSize();
//	return size;
//}

bool Transform::IsChildOf(entt::registry& aRegistry, entt::entity anEntity) const
{
	const Transform* t = this;

	do
	{
		if (t->myEntity == anEntity)
			return true;

		t = aRegistry.try_get<Transform>(t->myParent);
	}
	while (t);

	return false;
}

/*
* namespace ImGui
*/

void ImGui::Inspect(Transform& aTransform)
{
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

//
//bool ImGui::Hierarchy(Transform::Ptr aTransform, Transform::Ptr& aSelection)
//{
//	ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth };
//	if (!aTransform->HasChildren())
//		flags |= ImGuiTreeNodeFlags_Leaf;
//	if (aSelection == aTransform)
//		flags |= ImGuiTreeNodeFlags_Selected;
//
//	std::string name{ ICON_FA_CUBE };
//	name += " ";
//	name += aTransform->GetName();
//
//	const bool open = TreeNodeEx(name.c_str(), flags);
//
//	if (IsItemClicked() && !IsItemToggledOpen())
//		aSelection = aTransform;
//
//	bool invalidated = false;
//
//	if (BeginDragDropSource())
//	{
//		SetDragDropPayload("transform", &aTransform, sizeof(Transform::Ptr));
//		EndDragDropSource();
//	}
//
//	if (BeginDragDropTarget())
//	{
//		if (auto payload = AcceptDragDropPayload("transform"))
//		{
//			auto child = *reinterpret_cast<Transform::Ptr*>(payload->Data);
//			child->SetParent(aTransform);
//			invalidated = true;
//		}
//		EndDragDropTarget();
//	}
//
//	if (open)
//	{
//		if (!invalidated)
//		{
//			for (auto& child : aTransform->GetChildren())
//			{
//				if (Hierarchy(child, aSelection))
//				{
//					invalidated = true;
//					break;
//				}
//			}
//		}
//
//		TreePop();
//	}
//
//	return invalidated;
//}