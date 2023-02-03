#pragma once

class Transform : public std::enable_shared_from_this<Transform>
{
public:
	using Ptr = std::shared_ptr<Transform>;

	static Ptr Create();

	Ptr CreateChild();

	Ptr FindByName(std::string_view aName);
	std::vector<Ptr> FindAllByName(std::string_view aName);

	void Reset();

	void SetName(const std::string& aName)		{ myName = aName; }
	std::string_view GetName() const			{ return myName; }
	void SetLocalMatrix(const Matrix& aMatrix)	{ myLocalMatrix = aMatrix; }
	const Matrix& GetLocalMatrix()				{ return myLocalMatrix; }
	void SetWorldMatrix(const Matrix& aMatrix);
	Matrix GetWorldMatrix() const;

	Ptr GetParent() const;
	const std::vector<Ptr>& GetChildren() const { return myChildren; }

	size_t GetChildCount() const { return myChildren.size(); }
	size_t GetDescendantCount() const;
	size_t GetHierarchyCount() const { return GetDescendantCount() + 1; }

	bool HasParent() const { return myParent; }
	bool HasChildren() const { return !myChildren.empty(); }

	float* Data() { return myLocalMatrix.m[0]; }
	const float* Data() const { return myLocalMatrix.m[0]; }

private:
	Transform() = default;
	Transform(const Transform&) = delete;
	Transform& operator=(const Transform&) = delete;

	std::vector<Matrix> GetHierarchyWorldMatrices() const;
	std::span<Matrix> GetHierarchyWorldMatrices(std::span<Matrix>) const;

	Matrix myLocalMatrix;
	std::string myName;
	Transform* myParent = nullptr;
	std::vector<Ptr> myChildren;
};

namespace ImGui
{
	bool DragTransform(Transform::Ptr aTransform);
	bool ResetTransformButton(const char* aLabel, Transform::Ptr aTransform);
	void Hierarchy(Transform::Ptr aTransform, Transform::Ptr* aSelection = nullptr);
}

