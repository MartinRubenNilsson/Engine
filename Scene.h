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

	entt::entity Instantiate(entt::registry&) const;

private:
	void LoadMaterials(std::span<aiMaterial*>);
	void LoadMeshes(std::span<aiMesh*>);
	void LoadTransforms(Transform::Ptr aTransform, aiNode* aNode);
	void LoadCameras(std::span<aiCamera*>);
	void LoadLights(std::span<aiLight*>);

	Transform::Ptr myRootTransform{ Transform::Create() };
	std::vector<Material::Ptr> myMaterials{};
	std::vector<std::pair<Mesh::Ptr, unsigned>> myMeshes{};
	std::vector<std::pair<Transform::Ptr, unsigned>> myTransforms{};
	std::vector<std::pair<Camera, Transform::Ptr>> myCameras{};
	std::vector<std::pair<Light, Transform::Ptr>> myLights{};
};

class SceneManager : public Singleton<SceneManager>
{
public:
	std::shared_ptr<const Scene> GetScene(const fs::path&);

private:
	std::unordered_map<fs::path, std::shared_ptr<Scene>> myScenes;
};

