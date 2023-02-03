#include "pch.h"
#include "Transform.h"

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

std::vector<Transform::Ptr> Transform::FindAllByName(std::string_view aName)
{
	std::vector<Ptr> result{};
	if (aName == myName)
		result.emplace_back(shared_from_this());
	for (auto& child : myChildren)
		result.append_range(child->FindAllByName(aName)); // untested!!!
	return result;
}

void Transform::Reset()
{
	myLocalMatrix = Matrix::Identity;
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

Transform::Ptr Transform::GetParent() const
{
	return myParent ? myParent->shared_from_this() : nullptr;
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

bool ImGui::DragTransform(Transform::Ptr aTransform)
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

/*
* ImGui
*/

bool ImGui::ResetTransformButton(const char* aLabel, Transform::Ptr aTransform)
{
	const bool pushed = ImGui::Button(aLabel);
	if (pushed)
		aTransform->Reset();
	return pushed;
}

void ImGui::Hierarchy(Transform::Ptr aTransform, Transform::Ptr* aSelection)
{
	ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth };
	if (!aTransform->HasChildren())
		flags |= ImGuiTreeNodeFlags_Leaf;
	if (aSelection && *aSelection == aTransform)
		flags |= ImGuiTreeNodeFlags_Selected;

	const std::string name{ aTransform->GetName() };

	const bool open = TreeNodeEx(name.c_str(), flags);

	if (aSelection && IsItemClicked() && !IsItemToggledOpen())
		*aSelection = aTransform;

	if (open)
	{
		for (auto& child : aTransform->GetChildren())
			Hierarchy(child, aSelection);

		TreePop();
	}
}