#include "pch.h"
#include "Scene.h"
#include "Transform.h"
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"
#include "Light.h"

#pragma comment(lib, "assimp-vc142-mt")

#define IMPORT_FLAGS \
    aiProcess_ConvertToLeftHanded | \
    aiProcessPreset_TargetRealtime_MaxQuality | \
    static_cast<unsigned>(aiProcess_GenBoundingBoxes) 

Scene::Scene(const fs::path& aPath)
    : myPath{ aPath }
{
    Assimp::Importer importer{};
    const aiScene* scene{ importer.ReadFile(aPath.string().c_str(), IMPORT_FLAGS) };
    if (!scene)
        return;

    // 1. Load hierarchy

    Transform& root = Transform::CreateHierarchy(myRegistry, scene->mRootNode);

    // 2. Load cameras

    for (aiCamera* camera : std::span{ scene->mCameras, scene->mNumCameras })
    {
        entt::entity entity = root.Find(myRegistry, camera->mName.C_Str());
        myRegistry.emplace<Camera>(entity, *camera);
    }

    // 3. Load lights

    for (aiLight* light : std::span{ scene->mLights, scene->mNumLights })
    {
        entt::entity entity = root.Find(myRegistry, light->mName.C_Str());
        myRegistry.emplace<Light>(entity, *light);
    }

    // 4. Load materials

    std::vector<Material> materials{};

    {
        fs::path currPath{ fs::current_path() };
        fs::current_path(aPath.parent_path()); // Material textures are stored relative to scene

        for (aiMaterial* material : std::span{ scene->mMaterials, scene->mNumMaterials })
            materials.emplace_back(*material);

        fs::current_path(currPath);
    }

    // 5. Load meshes

    std::vector<std::pair<Mesh, Material>> meshes{};

    for (aiMesh* mesh : std::span{ scene->mMeshes, scene->mNumMeshes })
        meshes.emplace_back(*mesh, materials.at(mesh->mMaterialIndex));

    // 6. Store everything in registry

    for (auto [entity, node] : myRegistry.view<aiNode*>().each())
    {
        for (unsigned meshIndex : std::span{ node->mMeshes, node->mNumMeshes })
        {
            auto& [mesh, material] = meshes.at(meshIndex);
            myRegistry.emplace<Mesh>(entity, mesh);
            myRegistry.emplace<Material>(entity, material);
        }

        myRegistry.erase<aiNode*>(entity);
    }
}

Scene::operator bool() const
{
    return !myRegistry.empty();
}
