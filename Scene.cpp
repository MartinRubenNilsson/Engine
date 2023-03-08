#include "pch.h"
#include "Scene.h"
#pragma comment(lib, "assimp-vc142-mt")

#define IMPORT_FLAGS \
    aiProcess_ConvertToLeftHanded | \
    aiProcessPreset_TargetRealtime_MaxQuality | \
    static_cast<unsigned>(aiProcess_GenBoundingBoxes) 

/*
* class Scene
*/

Scene::Scene(const fs::path& aPath)
{
    Assimp::Importer importer{};
    const aiScene* scene{ importer.ReadFile(aPath.string().c_str(), IMPORT_FLAGS) };
    if (!scene)
        return;

    const fs::path prevPath{ fs::current_path() };
    fs::current_path(aPath.parent_path()); // Since textures are relative to scene's directory

    LoadMaterials({ scene->mMaterials, scene->mNumMaterials });
    LoadMeshes({ scene->mMeshes, scene->mNumMeshes });
    LoadTransforms(myRootTransform, scene->mRootNode);
    LoadCameras({ scene->mCameras, scene->mNumCameras });
    LoadLights({ scene->mLights, scene->mNumLights });

    fs::current_path(prevPath);

    myPath = aPath;
    mySucceeded = true;
}

entt::entity Scene::CopyTo(entt::registry& aRegistry) const
{
    return DeepCopy(aRegistry, myRootTransform).entity();
}

Scene::operator bool() const
{
    return mySucceeded;
}

void Scene::LoadMaterials(std::span<aiMaterial*> someMaterials)
{
    for (aiMaterial* material : someMaterials)
        myMaterials.emplace_back(*material);
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
            handle.emplace<Material>(material);
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

    auto scene{ std::make_shared<Scene>(aPath) };
    if (!*scene)
        return nullptr;

    myScenes.emplace(aPath, scene);
    return scene;
}
