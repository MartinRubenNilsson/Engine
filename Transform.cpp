#include "pch.h"
#include "Transform.h"

Transform::Ptr Transform::Create()
{
	return Ptr(new Transform());
}

Transform::Ptr Transform::CreateChild()
{
	auto child = myChildren.emplace_back(Create());
	child->myParent = weak_from_this();
	return child;
}

Transform::Ptr Transform::FindByName(std::string_view aName)
{
	if (aName == myName)
		return shared_from_this();
	for (auto& child : myChildren)
	{
		if (auto result = child->FindByName(aName))
			return result;
	}
	return nullptr;
}

Matrix Transform::GetWorldMatrix() const
{
	Matrix result = myLocalMatrix;
	if (!myParent.expired())
		result *= myParent.lock()->GetWorldMatrix();
	return result;
}

std::span<Matrix> Transform::GetHierarchyWorldMatrices(std::span<Matrix> aSpan) const
{
	std::memcpy(aSpan.data(), &myLocalMatrix, sizeof(Matrix));
	aSpan = aSpan.subspan(1);

	const auto descendants = aSpan;

	for (const auto& child : myChildren)
		aSpan = child->GetHierarchyWorldMatrices(aSpan);

	for (Matrix& matrix : descendants)
		matrix *= myLocalMatrix;
	
	return aSpan;
}

std::vector<Matrix> Transform::GetHierarchyWorldMatrices() const
{
	std::vector<Matrix> matrices(GetHierarchyCount());
	GetHierarchyWorldMatrices(matrices);
	return matrices;
}

size_t Transform::GetDescendantCount() const
{
	size_t count = GetChildCount();
	for (const auto& child : myChildren)
		count += child->GetDescendantCount();
	return count;
}

IMGUI_API void ImGui::Hierarchy(const char* label, Transform::Ptr t)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (!t->HasChildren())
		flags = flags | ImGuiTreeNodeFlags_Leaf;

	PushID(label);
	if (TreeNodeEx(t->GetName().c_str(), flags))
	{
		for (const auto& child : t->GetChildren())
			Hierarchy(label, child);
		TreePop();
	}
	PopID();
}