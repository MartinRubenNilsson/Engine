#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"

class Scene
{
public:
	Scene(const aiScene&);

private:
	using TransformPtr = std::shared_ptr<Transform>;
	using MeshPtr = std::shared_ptr<Mesh>;

	void LoadHierarchy(TransformPtr aTransform, const aiNode* aNode);

	std::vector<MeshPtr> myMeshes;
	TransformPtr myRootTransform;
	std::vector<std::pair<TransformPtr, MeshPtr>> myMeshInstances;
	std::vector<std::pair<TransformPtr, Camera>> myCameras;
};

