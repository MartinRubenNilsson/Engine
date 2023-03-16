#pragma once

class Scene
{
public:
	Scene() = default;
	Scene(const fs::path&);

	const fs::path& GetPath() const { return myPath; }
	const entt::registry& GetRegistry() const { return myRegistry; }

	explicit operator bool() const;

private:
	void ImportAsset(const fs::path&);
	void ImportArchive(const fs::path&);

	fs::path myPath{};
	entt::registry myRegistry{};
};

using SceneFactory = AssetFactory<Scene>;

