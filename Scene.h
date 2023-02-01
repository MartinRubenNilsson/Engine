#pragma once
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"

class Scene
{
public:
	Scene(const aiScene&);

	void ImGui();
	void Render() const;

private:
	using MeshPtr = std::shared_ptr<Mesh>;
	using TransformPtr = std::shared_ptr<Transform>;

	void LoadHierarchy(TransformPtr aTransform, const aiNode* aNode);

	std::vector<MeshPtr> myMeshes;
	TransformPtr myRootTransform;
	std::vector<std::pair<TransformPtr, MeshPtr>> myMeshInstances;
	std::vector<std::pair<TransformPtr, Camera>> myCameras;
};

