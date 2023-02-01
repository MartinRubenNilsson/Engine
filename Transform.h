#pragma once

class Transform : public std::enable_shared_from_this<Transform>
{
public:
	using Ptr = std::shared_ptr<Transform>;
	using WeakPtr = std::weak_ptr<Transform>;

	static Ptr Create();

	Ptr CreateChild();

	Ptr FindByName(std::string_view aName);

	void SetName(const std::string& aName)		{ myName = aName; }
	const std::string& GetName() const			{ return myName; }
	void SetLocalMatrix(const Matrix& aMatrix)	{ myLocalMatrix = aMatrix; }
	const Matrix& GetLocalMatrix()				{ return myLocalMatrix; }
	// void SetWorldMatrix(const Matrix& aMatrix) // todo;
	Matrix GetWorldMatrix() const;

	WeakPtr GetParent() const { return myParent; }
	const std::vector<Ptr>& GetChildren() const { return myChildren; }

	size_t GetChildCount() const { return myChildren.size(); }
	size_t GetDescendantCount() const;
	size_t GetHierarchyCount() const { return GetDescendantCount() + 1; }

	bool HasParent() const { return !myParent.expired(); }
	bool HasChildren() const { return !myChildren.empty(); }

private:
	Transform() = default;
	Transform(const Transform&) = delete;
	Transform& operator=(const Transform&) = delete;

	std::vector<Matrix> GetHierarchyWorldMatrices() const;
	std::span<Matrix> GetHierarchyWorldMatrices(std::span<Matrix>) const;

	Matrix myLocalMatrix;
	std::string myName;
	WeakPtr myParent;
	std::vector<Ptr> myChildren;
};

namespace ImGui
{
	IMGUI_API void Hierarchy(const char* label, Transform::Ptr t);
}

