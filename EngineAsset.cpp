#include "pch.h"
#include "EngineAsset.h"

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
