#include "pch.h"
#include "Transform.h"

Transform::Transform(const aiNode& aNode)
	: myName{ aNode.mName.C_Str() }
	, myLocalMatrix{}
	, myParent{}
	, myChildren{}
{
	std::memcpy(&myLocalMatrix, &aNode.mTransformation, sizeof(Matrix));
	myLocalMatrix = myLocalMatrix.Transpose();
	
	myChildren.resize(aNode.mNumChildren);
	for (unsigned i = 0; i < aNode.mNumChildren; ++i)
	{
		myChildren[i] = Ptr(new Transform(*aNode.mChildren[i]));
		myChildren[i]->myParent = shared_from_this();
	}
}

Transform::Ptr Transform::Create(const aiNode& aNode)
{
	return Ptr(new Transform(aNode));
}

Transform::Ptr Transform::FindInHierarchy(std::string_view aName)
{
	if (aName == myName)
		return shared_from_this();
	for (auto& child : myChildren)
	{
		if (auto result = child->FindInHierarchy(aName))
			return result;
	}
	return nullptr;
}

Matrix Transform::GetWorldMatrix() const
{
	Matrix result = myLocalMatrix;
	if (myParent)
		result *= myParent->GetWorldMatrix();
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