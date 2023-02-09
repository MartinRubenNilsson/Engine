#include "pch.h"
#include "Scene.h"
#pragma comment(lib, "assimp-vc142-mt")

/*
* class Scene
*/

Scene::Scene(const aiScene& aScene)
    : myRootTransform{ Transform::Create() }
    , myMeshes{}
    , myCameras{}
{
    LoadMeshes({ aScene.mMeshes, aScene.mNumMeshes });
    LoadHierarchy(myRootTransform, aScene.mRootNode);
    LoadCameras({ aScene.mCameras, aScene.mNumCameras });
}

void Scene::ImGui()
{
    static Transform::Ptr selection;

    if (ImGui::Begin("Hierarchy"))
        ImGui::Hierarchy(myRootTransform, selection);
    ImGui::End();

    if (selection)
    {
        if (ImGui::Begin("Inspector"))
        {
            if (ImGui::CollapsingHeader("Transform"))
            {
                ImGui::DragTransform(selection);
                ImGui::ResetTransformButton("Reset", selection);
            }
        }
        ImGui::End();
    }

    if (myCameras.empty())
        return;

    auto& [camera, transform] = myCameras.front();

    if (selection)
    {
        auto operation = ImGuizmo::TRANSLATE | ImGuizmo::ROTATE | ImGuizmo::SCALE;
        auto mode = ImGuizmo::LOCAL;

        Matrix m = selection->GetWorldMatrix();
        ImGui::Manipulate(camera, transform->GetWorldMatrix(), operation, mode, m);
        selection->SetWorldMatrix(m);
    }
}

void Scene::LoadMeshes(std::span<aiMesh*> someMeshes)
{
    for (aiMesh* mesh : someMeshes)
        myMeshes.emplace_back(*mesh, 0);
}

void Scene::LoadHierarchy(Transform::Ptr aTransform, aiNode* aNode)
{
    aTransform->SetName(aNode->mName.C_Str());

    std::memcpy(aTransform->Data(), &aNode->mTransformation.Transpose(), sizeof(Matrix));

    for (unsigned meshIndex : std::span{ aNode->mMeshes, aNode->mNumMeshes })
        myMeshes[meshIndex].second.emplace_back(aTransform);

    for (aiNode* childNode : std::span{ aNode->mChildren, aNode->mNumChildren })
        LoadHierarchy(aTransform->CreateChild(), childNode);
}

void Scene::LoadCameras(std::span<aiCamera*> someCameras)
{
    for (aiCamera* camera : someCameras)
        myCameras.emplace_back(*camera, myRootTransform->FindByName(camera->mName.C_Str()));
}

/*
* class SceneManager
*/

std::shared_ptr<Scene> SceneManager::GetScene(const fs::path& aPath)
{
    auto itr = myScenes.find(aPath);
    if (itr != myScenes.end())
        return itr->second;

    Assimp::Importer importer{};
    unsigned flags = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality;

    if (const aiScene* scene = importer.ReadFile(aPath.string().c_str(), flags))
    {
        itr = myScenes.emplace(aPath, std::make_shared<Scene>(*scene)).first;
        return itr->second;
    }

    return nullptr;
}
