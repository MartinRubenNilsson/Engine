#pragma once
#include "Transform.h"
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"
#include "Light.h"

class Scene
{
public:
	Scene() = default;
	Scene(const fs::path&);

	entt::entity CopyTo(entt::registry&) const; // Returns root entity

	const fs::path& GetPath() const { return myPath; }

	explicit operator bool() const;

private:
	void LoadMaterials(std::span<aiMaterial*>);
	void LoadMeshes(std::span<aiMesh*>);
	void LoadTransforms(Transform::Ptr, aiNode*); // Recursive
	void LoadCameras(std::span<aiCamera*>);
	void LoadLights(std::span<aiLight*>);

	entt::handle DeepCopy(entt::registry&, Transform::Ptr) const; // Recursive

	// entt::registry myRegistry{} // todo

	Transform::Ptr myRootTransform{ Transform::Create() };
	std::vector<Material> myMaterials{};
	std::vector<std::pair<Mesh::Ptr, unsigned>> myMeshes{};
	std::vector<std::pair<Transform::Ptr, unsigned>> myTransforms{};
	std::vector<std::pair<Camera, Transform::Ptr>> myCameras{};
	std::vector<std::pair<Light, Transform::Ptr>> myLights{};
	fs::path myPath{};
	bool mySucceeded{};
};

using SceneFactory = AssetFactory<Scene>;

