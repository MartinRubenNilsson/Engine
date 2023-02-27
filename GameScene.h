#pragma once
#include "Transform.h"

class GameScene
{
public:
	GameScene();

	entt::registry& GetRegistry() { return myRegistry; }

private:
	void OnConstructTransform(entt::registry&, entt::entity);
	void OnDestroyTransform(entt::registry&, entt::entity);

	entt::registry myRegistry{};
	std::unordered_map<Transform::Ptr, entt::entity> myTransformToEntity{};
};

