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

	operator bool() const { return mySuccessful; }

private:
	void LoadScene(const aiScene&);
	void LoadHierarchy(Transform::Ptr aTransform, const aiNode* aNode);

	fs::path myPath;
	Transform::Ptr myRootTransform;
	std::vector<std::shared_ptr<Mesh>> myMeshes;
	std::vector<std::pair<Transform::Ptr, std::shared_ptr<Mesh>>> myMeshInstances;
	std::vector<std::pair<Transform::Ptr, Camera>> myCameras;
	bool mySuccessful;
};

