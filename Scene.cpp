#include "pch.h"
#include "Scene.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "EnttCommon.h"

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

    ScopedCurrentPath scopedPath{ aPath.parent_path() }; // Since texture paths are relative to scene

    // 1. Load materials

    std::vector<Material> materials{};

    for (aiMaterial* material : std::span{ scene->mMaterials, scene->mNumMaterials })
        materials.emplace_back(*material);

    // 2. Load meshes and their material indices

    std::vector<Mesh> meshes{};
    std::vector<unsigned> meshToMatIndex{};

    for (aiMesh* mesh : std::span{ scene->mMeshes, scene->mNumMeshes })
    {
        meshes.emplace_back(*mesh).myPath = aPath;
        meshToMatIndex.push_back(mesh->mMaterialIndex);
    }

    // 3. Load hierarchy and emplace materials and meshes

    using Pair = std::pair<Transform*, aiNode*>;
    std::vector<Pair> stack{ { &Transform::Create(myRegistry), scene->mRootNode } };

    while (!stack.empty())
    {
        auto [transform, node] = stack.back();
        stack.pop_back();

        entt::entity entity = transform->GetEntity();
        auto& name = myRegistry.emplace<std::string>(entity, node->mName.C_Str());

        Matrix matrix{};
        std::memcpy(&matrix, &node->mTransformation, sizeof(Matrix));
        matrix.Transpose(matrix);

        transform->SetMatrix(matrix);

        for (unsigned meshIndex : std::span{ node->mMeshes, node->mNumMeshes })
        {
            // todo: if nMumMeshes > 1, then this will result in a crash!

            unsigned matIndex = meshToMatIndex.at(meshIndex);
            myRegistry.emplace<Material>(entity, materials.at(matIndex));
            myRegistry.emplace<Mesh>(entity, meshes.at(meshIndex)).myName = name;
        }

        for (aiNode* child : std::span{ node->mChildren, node->mNumChildren })
            stack.emplace_back(&transform->CreateChild(myRegistry), child);
    }

    // 2. Load and emplace cameras

    for (aiCamera* camera : std::span{ scene->mCameras, scene->mNumCameras })
    {
        entt::entity entity = Find(myRegistry, camera->mName.C_Str());
        myRegistry.emplace<Camera>(entity, *camera);
    }

    // 3. Load and emplace lights

    for (aiLight* light : std::span{ scene->mLights, scene->mNumLights })
    {
        entt::entity entity = Find(myRegistry, light->mName.C_Str());
        myRegistry.emplace<Light>(entity, *light);
    }
}

entt::entity Scene::Instantiate(entt::registry& aRegistry) const
{
    if (myRegistry.empty())
        return entt::null;

    auto& rootCopy = Transform::Create(aRegistry);
    auto& rootOrig = myRegistry.get<Transform>(GetRootEntity());

    std::vector<std::pair<Transform*, const Transform*>> stack{ { &rootCopy, &rootOrig } };

    while (!stack.empty())
    {
        auto [copy, orig] = stack.back();
        stack.pop_back();

        copy->position = orig->position;
        copy->rotation = orig->rotation;
        copy->scale = orig->scale;

        for (entt::entity child : orig->GetChildren())
        {
            auto& childCopy = copy->CreateChild(aRegistry);
            auto& childOrig = myRegistry.get<Transform>(child);
            stack.emplace_back(&childCopy, &childOrig);
        }

        entt::handle dst{ aRegistry, copy->GetEntity() };
        entt::const_handle src{ myRegistry, orig->GetEntity() };

        TryCopy<std::string>(dst, src);
        TryCopy<Material>(dst, src);
        TryCopy<Mesh>(dst, src);
        TryCopy<Camera>(dst, src);
        TryCopy<Light>(dst, src);
    }

    return rootCopy.GetEntity();
}

Scene::operator bool() const
{
    return !myRegistry.empty();
}

entt::entity Scene::GetRootEntity() const
{
    for (auto [entity, transform] : myRegistry.view<Transform>().each())
    {
        if (!transform.HasParent(myRegistry))
            return entity;
    }
    return entt::null;
}
