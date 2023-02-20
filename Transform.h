#pragma once

class Transform : public std::enable_shared_from_this<Transform>
{
public:
	using Ptr = std::shared_ptr<Transform>;

	static Ptr Create();

	~Transform();

	/*Ptr DeepCopy() const;*/
	Ptr CreateChild();
	Ptr Find(std::string_view aName);

	void SetName(std::string_view aName)		{ myName = aName; }
	std::string_view GetName() const			{ return myName; }
	void SetLocalMatrix(const Matrix& aMatrix)	{ myLocalMatrix = aMatrix; }
	const Matrix& GetLocalMatrix()				{ return myLocalMatrix; }
	void SetWorldMatrix(const Matrix& aMatrix);
	Matrix GetWorldMatrix() const;

	void SetParent(Ptr aParent, bool aWorldPositionStays = true);
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

	Matrix myLocalMatrix{};
	std::string myName{};
	Transform* myParent{ nullptr };
	std::vector<Ptr> myChildren{};
};

namespace ImGui
{
	bool InspectTransform(Transform::Ptr aTransform);
	bool Hierarchy(Transform::Ptr aTransform, Transform::Ptr& aSelection);
}

