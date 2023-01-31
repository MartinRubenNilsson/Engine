#include "pch.h"
#include "Transform.h"

Transform::Transform(const aiNode& aNode)
	: myName{ aNode.mName.C_Str() }
	, myMatrix{}
	, myChildren{}
{
	std::memcpy(&myMatrix, &aNode.mTransformation, sizeof(Matrix));
	myMatrix = myMatrix.Transpose();
	
	myChildren.resize(aNode.mNumChildren);
	for (unsigned i = 0; i < aNode.mNumChildren; ++i)
		myChildren[i] = std::make_shared<Transform>(*aNode.mChildren[i]);
}

std::span<Matrix> Transform::GetHierarchyWorldMatrices(std::span<Matrix> aSpan) const
{
	std::memcpy(aSpan.data(), &myMatrix, sizeof(Matrix));
	aSpan = aSpan.subspan(1);

	const auto descendants = aSpan;

	for (const auto& child : myChildren)
		aSpan = child->GetHierarchyWorldMatrices(aSpan);

	for (Matrix& matrix : descendants)
		matrix *= myMatrix;
	
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