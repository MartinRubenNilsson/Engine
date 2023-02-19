#include "pch.h"
#include "Transform.h"

/*
* class Transform
*/

Transform::Ptr Transform::Create()
{
	return Ptr(new Transform());
}

Transform::Ptr Transform::CreateChild()
{
	auto child = myChildren.emplace_back(Create());
	child->myParent = this;
	return child;
}

Transform::Ptr Transform::Find(std::string_view aName)
{
	if (aName == myName)
		return shared_from_this();
	for (auto& child : myChildren)
	{
		if (auto result = child->Find(aName))
			return result;
	}
	return nullptr;
}

void Transform::SetWorldMatrix(const Matrix& aMatrix)
{
	myLocalMatrix = aMatrix;
	if (myParent)
		myLocalMatrix *= myParent->GetWorldMatrix().Invert();
}

Matrix Transform::GetWorldMatrix() const
{
	Matrix result = myLocalMatrix;
	if (myParent)
		result *= myParent->GetWorldMatrix();
	return result;
}

void Transform::SetParent(Ptr aParent)
{
	if (!aParent)
		return;
	auto me = shared_from_this();
	if (aParent->IsChildOf(me))
		return;
	aParent->myChildren.push_back(me);
	if (myParent)
	{
		auto& siblings = myParent->myChildren;
		siblings.erase(std::remove(siblings.begin(), siblings.end(), me));
	}
	Matrix worldMatrix = GetWorldMatrix();
	myParent = aParent.get();
	SetWorldMatrix(worldMatrix);
}

Transform::Ptr Transform::GetParent() const
{
	return myParent ? myParent->shared_from_this() : nullptr;
}

size_t Transform::GetTreeSize() const
{
	size_t size = 1;
	for (const auto& child : myChildren)
		size += child->GetTreeSize();
	return size;
}

bool Transform::IsChildOf(Ptr aParent) const
{
	for (auto t = this; t->myParent; t = t->myParent)
	{
		if (t == aParent.get())
			return true;
	}
	return false;
}

/*
* namespace ImGui
*/

bool ImGui::InspectTransform(Transform::Ptr aTransform)
{
	float translation[3]{};
	float rotation[3]{};
	float scale[3]{};
	ImGuizmo::DecomposeMatrixToComponents(aTransform->Data(), translation, rotation, scale);
	const bool translated = ImGui::DragFloat3("Translation", translation, 0.025f);
	const bool rotated = ImGui::DragFloat3("Rotation", rotation, 0.25f);
	const bool scaled = ImGui::DragFloat3("Scale", scale, 0.025f);
	ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, aTransform->Data());
	return translated || rotated || scaled;
}

bool ImGui::Hierarchy(Transform::Ptr aTransform, Transform::Ptr& aSelection)
{
	ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth };
	if (!aTransform->HasChildren())
		flags |= ImGuiTreeNodeFlags_Leaf;
	if (aSelection == aTransform)
		flags |= ImGuiTreeNodeFlags_Selected;

	std::string name{ ICON_FA_CUBE };
	name += " ";
	name += aTransform->GetName();

	const bool open = TreeNodeEx(name.c_str(), flags);

	if (IsItemClicked() && !IsItemToggledOpen())
		aSelection = aTransform;

	bool invalidated = false;

	if (BeginDragDropSource())
	{
		SetDragDropPayload("transform", &aTransform, sizeof(Transform::Ptr));
		EndDragDropSource();
	}

	if (BeginDragDropTarget())
	{
		if (auto payload = AcceptDragDropPayload("transform"))
		{
			auto child = *reinterpret_cast<Transform::Ptr*>(payload->Data);
			child->SetParent(aTransform);
			invalidated = true;
		}
		EndDragDropTarget();
	}

	if (open)
	{
		if (!invalidated)
		{
			for (auto& child : aTransform->GetChildren())
			{
				if (Hierarchy(child, aSelection))
				{
					invalidated = true;
					break;
				}
			}
		}

		TreePop();
	}

	return invalidated;
}