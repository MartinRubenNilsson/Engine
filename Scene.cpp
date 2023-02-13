#include "pch.h"
#include "Scene.h"
#pragma comment(lib, "assimp-vc142-mt")

/*
* class Scene
*/

Scene::Scene(const aiScene& aScene)
{
    LoadMaterials({ aScene.mMaterials, aScene.mNumMaterials });
    LoadMeshes({ aScene.mMeshes, aScene.mNumMeshes });
    LoadTransforms(myRootTransform, aScene.mRootNode);
    LoadCameras({ aScene.mCameras, aScene.mNumCameras });
    LoadLights({ aScene.mLights, aScene.mNumLights });
}

entt::entity Scene::Instantiate(entt::registry& aRegistry) const
{
    for (auto& [transform, meshIndex] : myTransforms)
    {
        auto& [mesh, materialIndex] = myMeshes.at(meshIndex);
        auto& material = myMaterials.at(materialIndex);

        entt::handle handle{ aRegistry, aRegistry.create() };
        handle.emplace<Transform::Ptr>(transform);
        handle.emplace<Mesh::Ptr>(mesh);
        handle.emplace<Material::Ptr>(material);
    }

    for (auto& [camera, transform] : myCameras)
    {
        entt::handle handle{ aRegistry, aRegistry.create() };
        handle.emplace<Transform::Ptr>(transform);
        handle.emplace<Camera>(camera);
    }

    for (auto& [light, transform] : myCameras)
    {
        entt::handle handle{ aRegistry, aRegistry.create() };
        handle.emplace<Transform::Ptr>(transform);
        handle.emplace<Light>(light);
    }

    entt::entity entity{ aRegistry.create() };
    aRegistry.emplace<Transform::Ptr>(entity, myRootTransform);

    return entity;
}

void Scene::LoadMaterials(std::span<aiMaterial*> someMaterials)
{
    for (aiMaterial* material : someMaterials)
        myMaterials.emplace_back(std::make_shared<Material>(*material));
}

void Scene::LoadMeshes(std::span<aiMesh*> someMeshes)
{
    for (aiMesh* mesh : someMeshes)
        myMeshes.emplace_back(std::make_shared<Mesh>(*mesh), mesh->mMaterialIndex);
}

void Scene::LoadTransforms(Transform::Ptr aTransform, aiNode* aNode)
{
    aTransform->SetName(aNode->mName.C_Str());

    std::memcpy(aTransform->Data(), &aNode->mTransformation.Transpose(), sizeof(Matrix));

    for (unsigned meshIndex : std::span{ aNode->mMeshes, aNode->mNumMeshes })
        myTransforms.emplace_back(aTransform, meshIndex);

    for (aiNode* child : std::span{ aNode->mChildren, aNode->mNumChildren })
        LoadTransforms(aTransform->CreateChild(), child);
}

void Scene::LoadCameras(std::span<aiCamera*> someCameras)
{
    for (aiCamera* camera : someCameras)
        myCameras.emplace_back(*camera, myRootTransform->FindByName(camera->mName.C_Str()));
}

void Scene::LoadLights(std::span<aiLight*> someLights)
{
    for (aiLight* light : someLights)
        myLights.emplace_back(*light, myRootTransform->FindByName(light->mName.C_Str()));
}

/*
* class SceneManager
*/

std::shared_ptr<const Scene> SceneManager::GetScene(const fs::path& aPath)
{
    auto itr = myScenes.find(aPath);
    if (itr != myScenes.end())
        return itr->second;

    Assimp::Importer importer{};
    unsigned flags = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality;

    const aiScene* importedScene = importer.ReadFile(aPath.string().c_str(), flags);
    if (!importedScene)
        return nullptr;

    fs::path currentPath = fs::current_path();
    fs::current_path(aPath.parent_path());

    auto scene = std::make_shared<Scene>(*importedScene);
    myScenes.emplace(aPath, scene);

    fs::current_path(currentPath);

    return scene;
}
