#pragma once

class Transform
{
public:
	static Transform& Create(entt::registry&);

	Vector3 position{}; // Local position relative parent transform
	Quaternion rotation{}; // Local rotation relative parent transform
	Vector3 scale{ Vector3::One }; // Local scale relative parent transform

	Transform& CreateChild(entt::registry&);
	Transform& Duplicate(entt::registry&, entt::entity aParent = entt::null, bool aWorldTransformStays = true);
	void Destroy(entt::registry&);

	size_t GetDepth(const entt::registry&) const;

	entt::entity GetEntity() const { return myEntity; }

	void SetMatrix(Matrix); // Set local matrix relative parent transform
	Matrix GetMatrix() const; // Get local matrix relative parent transform
	void SetWorldMatrix(const entt::registry&, Matrix);
	Matrix GetWorldMatrix(const entt::registry&) const;

	void SetWorldPosition(const entt::registry&, const Vector3&);

	void SetParent(entt::registry&, entt::entity aParent = entt::null, bool aWorldTransformStays = true);
	entt::entity GetParent() const { return myParent; }

	const auto& GetChildren() const { return myChildren; }

	bool IsChildOf(const entt::registry&, entt::entity) const; // Also true if equal
	bool HasParent(const entt::registry&) const;
	bool HasChildren() const { return !myChildren.empty(); }

private:
	void AddChild(entt::entity);
	void RemoveChild(entt::entity);

	friend void from_json(const json&, Transform&);
	friend void to_json(json&, const Transform&);

	entt::entity myEntity{ entt::null }, myParent{ entt::null };
	std::vector<entt::entity> myChildren{};
};

namespace ImGui
{
	void Inspect(Transform&);
}

