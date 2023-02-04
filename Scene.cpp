#include "pch.h"
#include "Scene.h"
#pragma comment(lib, "assimp-vc142-mt")

Scene::Scene(const fs::path& aPath)
    : myPath{ aPath }
    , myRootTransform{ Transform::Create() }
    , myMeshes{}
    , myCameras{}
    , mySucceeded{ false }
{
    Assimp::Importer importer{};
    constexpr unsigned flags = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality;

    if (auto scene = importer.ReadFile(aPath.string().c_str(), flags))
    {
        LoadScene(*scene);
        mySucceeded = true;
    }
}

void Scene::ImGui()
{
    static Transform::Ptr selection;

    if (ImGui::Begin("Hierarchy"))
        ImGui::Hierarchy(myRootTransform, &selection);
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

    if (ImGui::Begin("Camera"))
        ImGui::CameraEdit(camera);
    ImGui::End();

    if (selection)
    {
        auto operation = ImGuizmo::TRANSLATE | ImGuizmo::ROTATE | ImGuizmo::SCALE;
        auto mode = ImGuizmo::LOCAL;

        Matrix m = selection->GetWorldMatrix();
        ImGui::Manipulate(camera, transform->GetWorldMatrix(), operation, mode, m);
        selection->SetWorldMatrix(m);
    }
}

void Scene::Render() const
{
    if (!myCameras.empty())
    {
        auto& [camera, transform] = myCameras.front();
        camera.SetCamera(transform->GetWorldMatrix());
    }

    for (auto& [mesh, transforms] : myMeshes)
    {
        for (auto& transform : transforms)
            mesh.Draw(transform->GetWorldMatrix());
    }
}

void Scene::LoadScene(const aiScene& aScene)
{
    LoadMeshes({ aScene.mMeshes, aScene.mNumMeshes });
    LoadHierarchy(myRootTransform, aScene.mRootNode);
    LoadCameras({ aScene.mCameras, aScene.mNumCameras });
}

void Scene::LoadMeshes(std::span<aiMesh*> someMeshes)
{
    for (aiMesh* mesh : someMeshes)
        myMeshes.emplace_back(*mesh, Transforms{});
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
