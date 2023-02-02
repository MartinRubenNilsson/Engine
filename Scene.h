#pragma once
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"

class Scene
{
public:
	Scene(const fs::path&);

	void ImGui();
	void Render() const;

	operator bool() const { return mySucceeded; }

private:
	using Transforms = std::vector<Transform::Ptr>;

	void LoadScene(const aiScene&);
	void LoadMeshes(std::span<aiMesh*>);
	void LoadHierarchy(Transform::Ptr aTransform, aiNode* aNode);
	void LoadCameras(std::span<aiCamera*>);

	fs::path myPath;
	Transform::Ptr myRootTransform;
	std::vector<std::pair<Mesh, Transforms>> myMeshes;
	std::vector<std::pair<Camera, Transform::Ptr>> myCameras;
	bool mySucceeded;
};

