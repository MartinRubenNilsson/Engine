#pragma once

class Transform
{
public:
	static Transform& CreateHierarchy(entt::registry&, aiNode*); // Recursive

	entt::entity Find(const entt::registry&, std::string_view aName) const;

	void Destroy(entt::registry&); // Destroys all children as well

	size_t GetDepth(const entt::registry&) const;

	entt::entity GetEntity() const { return myEntity; }

	void SetName(std::string_view aName)		{ myName = aName; }
	std::string_view GetName() const			{ return myName; }
	void SetLocalMatrix(const Matrix& aMatrix)	{ myLocalMatrix = aMatrix; }
	const Matrix& GetLocalMatrix()				{ return myLocalMatrix; }
	void SetWorldMatrix(const entt::registry&, const Matrix&);
	Matrix GetWorldMatrix(const entt::registry&) const;

	void SetParent(entt::registry&, entt::entity, bool aWorldTransformStays = true); // Pass entt::null to unparent
	entt::entity GetParent() const { return myParent; }

	const auto& GetChildren() const { return myChildren; }

	bool IsChildOf(entt::registry&, entt::entity) const; // Also true if equal
	bool HasChildren() const { return !myChildren.empty(); }

	float* Data() { return &myLocalMatrix._11; }
	const float* Data() const { return &myLocalMatrix._11; }

private:
	void AddChild(entt::entity);
	void RemoveChild(entt::entity);

	friend void from_json(const json&, Transform&);
	friend void to_json(json&, const Transform&);

	entt::entity myEntity{ entt::null }, myParent{ entt::null };
	std::vector<entt::entity> myChildren{};
	std::string myName{};
	Matrix myLocalMatrix{};
};

namespace ImGui
{
	void Inspect(Transform&);
}

