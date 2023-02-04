#pragma once
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"

class Scene
{
public:
	Scene(const fs::path&);

	void ImGui();

	const fs::path& GetPath() const { return myPath; }
	const auto& GetMeshes() const { return myMeshes; }
	const auto& GetCameras() const { return myCameras; }

	operator bool() const { return mySucceeded; }

private:
	void LoadScene(const aiScene&);
	void LoadMeshes(std::span<aiMesh*>);
	void LoadHierarchy(Transform::Ptr aTransform, aiNode* aNode);
	void LoadCameras(std::span<aiCamera*>);

	fs::path myPath;
	Transform::Ptr myRootTransform;
	std::vector<std::pair<Mesh, std::vector<Transform::Ptr>>> myMeshes;
	std::vector<std::pair<Camera, Transform::Ptr>> myCameras;
	bool mySucceeded;
};

