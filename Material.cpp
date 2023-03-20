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
    : myName{ aMaterial.GetName().C_Str() }
{
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
            SetTexture(texture);
    }
}

void Material::SetTexture(Texture::Ptr aTexture)
{
    if (!aTexture || !*aTexture)
        return;
    for (auto& texture : myTextures)
    {
        if (texture->GetType() == aTexture->GetType())
        {
            texture = aTexture;
            return;
        }
    }
    myTextures.push_back(aTexture);
}

Texture::Ptr Material::GetTexture(TextureType aType) const
{
    for (auto& texture : myTextures)
    {
        if (texture->GetType() == aType)
            return texture;
    }
    return nullptr;
}

std::vector<ShaderResourcePtr> Material::GetResources() const
{
    std::vector<ShaderResourcePtr> resources{};
    for (TextureType type : GetMaterialTextureTypes())
    {
        if (auto texture = GetTexture(type))
            resources.push_back(texture->GetResource());
        else
            resources.push_back(nullptr);
    }
    return resources;
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
    Spacing();

    for (TextureType type : GetMaterialTextureTypes())
    {
        Texture::Ptr texture = aMaterial.GetTexture(type);

        std::string label{ TextureTypeToString(type) };
        label += ": ";
        if (texture)
            label += texture->GetPath().filename().string();

        if (Selectable(label.c_str()))
        {
            // todo: open file explorer
        }

        if (texture && IsItemHovered())
        {
            BeginTooltip();
            Image(texture->GetResource().Get(), { 256.f, 256.f });
            EndTooltip();
        }
    }
}
