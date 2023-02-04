#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"

class Scene
{
public:
	Scene(const aiScene&);

	void ImGui();

	const auto& GetMeshes() const { return myMeshes; }
	const auto& GetCameras() const { return myCameras; }

private:
	void LoadMeshes(std::span<aiMesh*>);
	void LoadHierarchy(Transform::Ptr aTransform, aiNode* aNode);
	void LoadCameras(std::span<aiCamera*>);

	Transform::Ptr myRootTransform;
	std::vector<std::pair<Mesh, std::vector<Transform::Ptr>>> myMeshes;
	std::vector<std::pair<Camera, Transform::Ptr>> myCameras;
};

class SceneManager : public Singleton<SceneManager>
{
public:
	std::shared_ptr<Scene> GetScene(const fs::path&);

private:
	std::unordered_map<fs::path, std::shared_ptr<Scene>> myScenes;
};

