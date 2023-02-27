#pragma once
#include "Transform.h"
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"
#include "Light.h"

class Scene
{
public:
	Scene(const aiScene&);

	entt::entity CopyTo(entt::registry&) const; // Returns root entity

private:
	void LoadMaterials(std::span<aiMaterial*>);
	void LoadMeshes(std::span<aiMesh*>);
	void LoadTransforms(Transform::Ptr, aiNode*); // Recursive
	void LoadCameras(std::span<aiCamera*>);
	void LoadLights(std::span<aiLight*>);

	entt::handle DeepCopy(entt::registry&, Transform::Ptr) const; // Recursive

	Transform::Ptr myRootTransform{ Transform::Create() };
	std::vector<Material::Ptr> myMaterials{};
	std::vector<std::pair<Mesh::Ptr, unsigned>> myMeshes{};
	std::vector<std::pair<Transform::Ptr, unsigned>> myTransforms{};
	std::vector<std::pair<Camera, Transform::Ptr>> myCameras{};
	std::vector<std::pair<Light, Transform::Ptr>> myLights{};
};

class SceneFactory : public Singleton<SceneFactory>
{
public:
	std::shared_ptr<const Scene> GetScene(const fs::path&);

private:
	std::unordered_map<fs::path, std::shared_ptr<Scene>> myScenes;
};

