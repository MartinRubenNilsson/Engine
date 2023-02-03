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

    if (ImGui::Begin("Camera"))
    {
        if (!myCameras.empty())
        {
            auto& [camera, transform] = myCameras.front();
            ImGui::CameraEdit(&camera);

            Matrix world = transform->GetWorldMatrix();
            Matrix view = camera.GetWorldViewMatrix(transform->GetWorldMatrix());
            Matrix proj = camera.GetProjectionMatrix();

            Matrix identity{};

            ImGuizmo::DrawCubes(world.m[0], proj.m[0], identity.m[0], 1);
        }
    }
    ImGui::End();

    if (ImGui::Begin("Hierarchy"))
    {
        ImGui::Hierarchy(myRootTransform, &selection);
    }
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
    for (auto mesh : someMeshes)
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
    for (auto camera : someCameras)
        myCameras.emplace_back(*camera, myRootTransform->FindByName(camera->mName.C_Str()));
}
