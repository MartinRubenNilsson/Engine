#pragma once

class Transform
{
public:
	using Ptr = std::shared_ptr<Transform>;

	Transform(const aiNode&);

	const std::string& GetName() const { return myName; }

	const Matrix& GetMatrix() { return myMatrix; }
	void SetMatrix(const Matrix& aMatrix) { myMatrix = aMatrix; }

	std::vector<Matrix> GetHierarchyWorldMatrices() const;

	size_t GetChildCount() const { return myChildren.size(); }
	size_t GetDescendantCount() const;
	size_t GetHierarchyCount() const { return GetDescendantCount() + 1; }

	const std::vector<Ptr>& GetChildren() const { return myChildren; }

	bool HasChildren() const { return !myChildren.empty(); }

private:
	std::span<Matrix> GetHierarchyWorldMatrices(std::span<Matrix>) const;

	std::string myName;
	Matrix myMatrix;
	std::vector<Ptr> myChildren;
};

namespace ImGui
{
	IMGUI_API void Hierarchy(const char* label, Transform::Ptr t);
}

