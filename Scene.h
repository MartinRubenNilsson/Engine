#pragma once

class Scene
{
public:
	Scene() = default;
	Scene(const fs::path&);

	// Returns root entity
	entt::entity Instantiate(entt::registry&) const;

	entt::handle Find(std::string_view aName);
	entt::const_handle Find(std::string_view aName) const;

	const fs::path& GetPath() const { return myPath; }
	const entt::registry& GetRegistry() const { return myRegistry; }

	explicit operator bool() const;

private:
	entt::entity GetRootEntity() const;

	fs::path myPath{};
	entt::registry myRegistry{};
};

using SceneFactory = AssetFactory<Scene>;

