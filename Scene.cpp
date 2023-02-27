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

entt::entity Scene::CopyTo(entt::registry& aRegistry) const
{
    return DeepCopy(aRegistry, myRootTransform).entity();
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
        myCameras.emplace_back(*camera, myRootTransform->Find(camera->mName.C_Str()));
}

void Scene::LoadLights(std::span<aiLight*> someLights)
{
    for (aiLight* light : someLights)
        myLights.emplace_back(*light, myRootTransform->Find(light->mName.C_Str()));
}

entt::handle Scene::DeepCopy(entt::registry& aRegistry, Transform::Ptr aTransform) const
{
    entt::handle handle{ aRegistry, aRegistry.create() };
    
    for (auto& [transform, meshIndex] : myTransforms)
    {
        if (transform == aTransform)
        {
            auto& [mesh, materialIndex] = myMeshes.at(meshIndex);
            auto& material = myMaterials.at(materialIndex);
            handle.emplace<Mesh::Ptr>(mesh);
            handle.emplace<Material::Ptr>(material);
            break;
        }
    }

    for (auto& [camera, transform] : myCameras)
    {
        if (transform == aTransform)
        {
            handle.emplace<Camera>(camera);
            break;
        }
    }

    for (auto& [light, transform] : myLights)
    {
        if (transform == aTransform)
        {
            handle.emplace<Light>(light);
            break;
        }
    }

    auto& transform{ handle.emplace<Transform::Ptr>(Transform::Create()) };
    transform->SetName(aTransform->GetName());
    transform->SetLocalMatrix(aTransform->GetLocalMatrix());

    for (auto& child : aTransform->GetChildren())
        DeepCopy(aRegistry, child).get<Transform::Ptr>()->SetParent(transform, false);

    return handle;
}

/*
* class SceneFactory
*/

std::shared_ptr<const Scene> SceneFactory::GetScene(const fs::path& aPath)
{
    auto itr = myScenes.find(aPath);
    if (itr != myScenes.end())
        return itr->second;

    constexpr unsigned flags =
        aiProcess_ConvertToLeftHanded |
        aiProcessPreset_TargetRealtime_MaxQuality |
        static_cast<unsigned>(aiProcess_GenBoundingBoxes);
    
    Assimp::Importer importer{};
    const aiScene* importedScene = importer.ReadFile(aPath.string().c_str(), flags);
    if (!importedScene)
        return nullptr;

    fs::path currentPath{ fs::current_path() };
    fs::current_path(aPath.parent_path()); // Since textures are relative to scene's directory

    auto scene = std::make_shared<Scene>(*importedScene);
    myScenes.emplace(aPath, scene);

    fs::current_path(currentPath);

    return scene;
}
