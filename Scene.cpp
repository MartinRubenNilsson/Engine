#include "pch.h"
#include "Scene.h"
#include "imgui_simplemath.h"
#pragma comment(lib, "assimp-vc142-mt")

Scene::Scene(const fs::path& aPath)
    : myPath{ aPath }
    , myRootTransform{}
    , myMeshes{}
    , myMeshInstances{}
    , myCameras{}
    , mySuccessful{ false }
{
    Assimp::Importer importer{};
    const unsigned flags = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality;

    if (const aiScene* scene = importer.ReadFile(aPath.string().c_str(), flags))
    {
        mySuccessful = true;
        LoadScene(*scene);
    }
}


void Scene::LoadScene(const aiScene& aScene)
{
    for (const aiMesh* mesh : std::span{ aScene.mMeshes, aScene.mNumMeshes })
        myMeshes.emplace_back(std::make_shared<Mesh>(*mesh));

	myRootTransform = Transform::Create();
    LoadHierarchy(myRootTransform, aScene.mRootNode);

    for (const aiCamera* camera : std::span{ aScene.mCameras, aScene.mNumCameras })
    {
        Matrix cameraMatrix{};
        camera->GetCameraMatrix(reinterpret_cast<aiMatrix4x4&>(cameraMatrix));

        auto cameraTransform = myRootTransform->FindByName(camera->mName.C_Str());
        cameraTransform->SetLocalMatrix(cameraMatrix * cameraTransform->GetLocalMatrix());

        myCameras.emplace_back(cameraTransform, *camera);
    }
}

void Scene::ImGui()
{
    static Transform::Ptr selection;
    ImGui::Hierarchy(myRootTransform, &selection);

    if (selection)
        ImGui::DragTransform(selection);

    if (!myCameras.empty())
    {
        auto& [transform, camera] = myCameras.front();
        camera.UseForDrawing(transform->GetWorldMatrix());
    }
}

void Scene::Render() const
{
    if (!myCameras.empty())
    {
        auto& [transform, camera] = myCameras.front();
        camera.UseForDrawing(transform->GetWorldMatrix());
    }

    for (auto& [transform, mesh] : myMeshInstances)
        mesh->Draw(transform->GetWorldMatrix());
}

void Scene::LoadHierarchy(Transform::Ptr aTransform, const aiNode* aNode)
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
