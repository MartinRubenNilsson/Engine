#include "pch.h"
#include "Material.h"
#include "EngineAsset.h"

/*
* class Material
*/

Material::Material()
    : myTextures{
        TextureFactory::Get().GetAsset(GetPath(EngineAsset::DefaultAlbedo)),
        TextureFactory::Get().GetAsset(GetPath(EngineAsset::DefaultNormal)),
        TextureFactory::Get().GetAsset(GetPath(EngineAsset::DefaultMetallic)),
        TextureFactory::Get().GetAsset(GetPath(EngineAsset::DefaultRoughness)),
        TextureFactory::Get().GetAsset(GetPath(EngineAsset::DefaultOcclusion)),
    }
{
}

Material::Material(const aiMaterial& aMaterial)
    : Material()
{
    myName = aMaterial.GetName().C_Str();

    for (int i = aiTextureType_DIFFUSE; i < aiTextureType_UNKNOWN; ++i)
    {
        aiTextureType aiType = static_cast<aiTextureType>(i);

        if (aMaterial.GetTextureCount(aiType) == 0)
            continue;

        aiString path{};
        if (aMaterial.GetTexture(aiType, 0, &path) != aiReturn_SUCCESS)
            continue;

        TextureType type{};

        switch (aiType)
        {
        case aiTextureType_DIFFUSE:
            type = TextureType::Albedo;
            break;
        case aiTextureType_NORMALS:
            type = TextureType::Normal;
            break;
        case aiTextureType_METALNESS:
            type = TextureType::Metallic;
            break;
        case aiTextureType_SHININESS:
            type = TextureType::Roughness;
            break;
        default:
            Debug::Println(std::format(
                "Warning: Unsupported texture type {}: {}",
                TextureTypeToString(aiType), path.C_Str()
            ));
            continue;
        }

        if (auto texture = TextureFactory::Get().GetAsset(path.C_Str(), type))
            AddOrReplaceTexture(texture);
    }
}

void Material::AddOrReplaceTexture(Texture::Ptr aTexture)
{
    if (!aTexture)
        return;
    auto itr = std::ranges::find(myTextures, aTexture);
    if (itr != myTextures.end())
        *itr = aTexture;
    else
        myTextures.push_back(aTexture);
}

ShaderResourcePtr Material::GetResource(TextureType aType) const
{
    auto itr = std::ranges::find(myTextures, aType, [](Texture::Ptr ptr) { return ptr->GetType(); });
    return itr != myTextures.end() ? (*itr)->GetResource() : nullptr;
}

void to_json(json& j, const Material& m)
{
    j["name"] = m.myName;
    j["textures"] = m.myTextures;
}

void from_json(const json& j, Material& m)
{
    j.at("name").get_to(m.myName);
    j.at("textures").get_to(m.myTextures);
}

/*
* namespace ImGui
*/

void ImGui::Inspect(const Material& aMaterial)
{
    Text("Name: %s", aMaterial.GetName().data());

    for (auto& texture : aMaterial.GetTextures())
    {
        if (TreeNode(TextureTypeToString(texture->GetType())))
        {
            TextWrapped(texture->GetPath().string().c_str());
            Image(texture->GetResource().Get(), { 200.f, 200.f });
            TreePop();
        }
    }
}
