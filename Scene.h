#pragma once

class Scene
{
public:
	using Ptr = std::shared_ptr<const Scene>;

	Scene() = default;
	Scene(const fs::path&);

	const fs::path& GetPath() const { return myPath; }
	entt::registry& GetRegistry() const { return myRegistry; }

	explicit operator bool() const;

private:
	void ImportAsset(const fs::path&);
	void ImportArchive(const fs::path&);

	fs::path myPath{};
	mutable entt::registry myRegistry{};
};

using SceneFactory = AssetFactory<Scene>;

