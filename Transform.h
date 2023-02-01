#pragma once

class Transform : public std::enable_shared_from_this<Transform>
{
public:
	using Ptr = std::shared_ptr<Transform>;

	static Ptr Create(const aiNode&);

	Ptr FindInHierarchy(std::string_view aName);

	void SetName(const std::string& aName)		{ myName = aName; }
	const std::string& GetName() const			{ return myName; }
	void SetLocalMatrix(const Matrix& aMatrix)	{ myLocalMatrix = aMatrix; }
	const Matrix& GetLocalMatrix()				{ return myLocalMatrix; }
	// void SetWorldMatrix(const Matrix& aMatrix) // todo;
	Matrix GetWorldMatrix() const;

	std::vector<Matrix> GetHierarchyWorldMatrices() const;

	Ptr GetParent() const { return myParent; }
	const std::vector<Ptr>& GetChildren() const { return myChildren; }

	size_t GetChildCount() const { return myChildren.size(); }
	size_t GetDescendantCount() const;
	size_t GetHierarchyCount() const { return GetDescendantCount() + 1; }

	bool HasParent() const { return myParent.operator bool(); }
	bool HasChildren() const { return !myChildren.empty(); }

private:
	Transform(const aiNode&);

	std::span<Matrix> GetHierarchyWorldMatrices(std::span<Matrix>) const;

	std::string myName;
	Matrix myLocalMatrix;
	Ptr myParent;
	std::vector<Ptr> myChildren;
};

namespace ImGui
{
	IMGUI_API void Hierarchy(const char* label, Transform::Ptr t);
}

