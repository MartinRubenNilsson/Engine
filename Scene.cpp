#include "pch.h"
#include "Scene.h"

Scene::Scene(const aiScene& aScene)
{
    for (const aiMesh* mesh : std::span{ aScene.mMeshes, aScene.mNumMeshes })
        myMeshes.emplace_back(std::make_shared<Mesh>(*mesh));

	myRootTransform = Transform::Create();
    LoadHierarchy(myRootTransform, aScene.mRootNode);

    for (const aiCamera* camera : std::span{ aScene.mCameras, aScene.mNumCameras })
        myCameras.emplace_back(myRootTransform->FindByName(camera->mName.C_Str()), *camera);
}

void Scene::LoadHierarchy(TransformPtr aTransform, const aiNode* aNode)
{
    aTransform->SetName(aNode->mName.C_Str());

    {
        Matrix localMatrix{};
        std::memcpy(&localMatrix, &aNode->mTransformation, sizeof(Matrix));
        aTransform->SetLocalMatrix(localMatrix.Transpose());
    }

    for (unsigned meshIndex : std::span{ aNode->mMeshes, aNode->mNumMeshes })
        myMeshInstances.emplace_back(aTransform, myMeshes[meshIndex]);

    for (const aiNode* childNode : std::span{ aNode->mChildren, aNode->mNumChildren })
        LoadHierarchy(aTransform->CreateChild(), childNode);
}
