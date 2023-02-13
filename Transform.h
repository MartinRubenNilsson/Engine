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

	void SetParent(Ptr aParent);
	Ptr GetParent() const;

	const auto& GetChildren() const { return myChildren; }

	size_t GetChildCount() const { return myChildren.size(); }
	size_t GetTreeSize() const;

	bool IsChildOf(Ptr aParent) const; // true if this is identical to or a descendant of aParent, otherwise false.
	bool HasParent() const { return myParent; }
	bool HasChildren() const { return !myChildren.empty(); }

	float* Data() { return &myLocalMatrix._11; }
	const float* Data() const { return &myLocalMatrix._11; }

private:
	Transform() = default;
	Transform(const Transform&) = delete;
	Transform& operator=(const Transform&) = delete;
	Transform(Transform&&) = delete;
	Transform& operator=(Transform&&) = delete;

	/*std::vector<Matrix> GetHierarchyWorldMatrices() const;
	std::span<Matrix> GetHierarchyWorldMatrices(std::span<Matrix>) const;*/

	Matrix myLocalMatrix;
	std::string myName;
	Transform* myParent = nullptr;
	std::vector<Ptr> myChildren;
};

namespace ImGui
{
	bool InspectTransform(Transform::Ptr aTransform);
	bool Hierarchy(Transform::Ptr aTransform, Transform::Ptr& aSelection);
}

