#pragma once

class PhysicMaterial
{
public:
	PhysicMaterial();

	void SetEntity(entt::entity);
	entt::entity GetEntity() const;

	explicit operator bool() const;

private:
	PxPtr<PxMaterial> myImpl{};
};

