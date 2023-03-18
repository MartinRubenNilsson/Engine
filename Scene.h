#pragma once
#include "Mesh.h"

class Scene
{
public:
	Scene() = default;
	Scene(const fs::path&);

	const fs::path& GetPath() const { return myPath; }
	const entt::registry& GetRegistry() const { return myRegistry; }

	explicit operator bool() const;

private:
	fs::path myPath{};
	entt::registry myRegistry{};
};

using SceneFactory = AssetFactory<Scene>;

