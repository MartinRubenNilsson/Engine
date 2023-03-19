#include "pch.h"
#include "EngineAsset.h"
#include "Texture.h"
#include "Scene.h"

fs::path GetPath(EngineAsset anAsset)
{
    static constexpr std::array paths
    {
        "assets/engine/default_albedo.png",
        "assets/engine/default_normal.png",
        "assets/engine/default_metallic.png",
        "assets/engine/default_roughness.png",
        "assets/engine/default_occlusion.png",
        "assets/engine/shapes.fbx",
    };

    return { paths.at(std::to_underlying(anAsset)) };
}

bool ImportEngineAssets()
{
    auto& textureFactory = TextureFactory::Get();
    auto& sceneFactory = SceneFactory::Get();

    if (!textureFactory.GetAsset(GetPath(EngineAsset::DefaultAlbedo), TextureType::Albedo))
        return false;
    if (!textureFactory.GetAsset(GetPath(EngineAsset::DefaultNormal), TextureType::Normal))
        return false;
    if (!textureFactory.GetAsset(GetPath(EngineAsset::DefaultMetallic), TextureType::Metallic))
        return false;
    if (!textureFactory.GetAsset(GetPath(EngineAsset::DefaultRoughness), TextureType::Roughness))
        return false;
    if (!textureFactory.GetAsset(GetPath(EngineAsset::DefaultOcclusion), TextureType::Occlusion))
        return false;

    if (!sceneFactory.GetAsset(GetPath(EngineAsset::ShapesScene)))
        return false;

    return true;
}
