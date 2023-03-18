#include "pch.h"
#include "Scene.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"

#pragma comment(lib, "assimp-vc142-mt")

#define IMPORT_FLAGS \
    aiProcess_ConvertToLeftHanded | \
    aiProcessPreset_TargetRealtime_MaxQuality | \
    static_cast<unsigned>(aiProcess_GenBoundingBoxes) 

Scene::Scene(const fs::path& aPath)
{
    Assimp::Importer importer{};
    const aiScene* scene{ importer.ReadFile(aPath.string().c_str(), IMPORT_FLAGS) };
    if (!scene)
        return;

    ScopedCurrentPath scopedPath{ aPath.parent_path() };

    // 1. Load materials

    std::vector<Material> materials{};

    for (aiMaterial* material : std::span{ scene->mMaterials, scene->mNumMaterials })
        materials.emplace_back(*material);

    // 2. Load meshes and their material indices

    std::vector<Mesh> meshes{};
    std::vector<unsigned> meshToMaterialIndex{};

    for (aiMesh* mesh : std::span{ scene->mMeshes, scene->mNumMeshes })
    {
        meshes.emplace_back(aPath, *mesh);
        meshToMaterialIndex.push_back(mesh->mMaterialIndex);
    }

    // 3. Load hierarchy and emplace materials and meshes

    Transform& root = Transform::Create(myRegistry);

    {
        std::vector<std::pair<Transform*, aiNode*>> stack{ { &root, scene->mRootNode } };

        while (!stack.empty())
        {
            auto [transform, node] = stack.back();
            stack.pop_back();

            transform->SetName(node->mName.C_Str());
            std::memcpy(transform->Data(), &node->mTransformation.Transpose(), sizeof(Matrix));

            for (unsigned meshIndex : std::span{ node->mMeshes, node->mNumMeshes })
            {
                unsigned materialIndex = meshToMaterialIndex.at(meshIndex);
                myRegistry.emplace<Material>(transform->GetEntity(), materials.at(materialIndex));
                myRegistry.emplace<Mesh>(transform->GetEntity(), meshes.at(meshIndex));
            }

            for (aiNode* childNode : std::span{ node->mChildren, node->mNumChildren })
            {
                Transform& childTransform = transform->CreateChild(myRegistry);
                stack.emplace_back(&childTransform, childNode);
            }
        }
    }

    // 2. Load and emplace cameras

    for (aiCamera* camera : std::span{ scene->mCameras, scene->mNumCameras })
    {
        entt::entity entity = root.Find(myRegistry, camera->mName.C_Str());
        myRegistry.emplace<Camera>(entity, *camera);
    }

    // 3. Load and emplace lights

    for (aiLight* light : std::span{ scene->mLights, scene->mNumLights })
    {
        entt::entity entity = root.Find(myRegistry, light->mName.C_Str());
        myRegistry.emplace<Light>(entity, *light);
    }
}

Scene::operator bool() const
{
    return !myRegistry.empty();
}
