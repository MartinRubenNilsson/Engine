#include "pch.h"
#include "Material.h"
#include "EngineAsset.h"

namespace
{
    TextureType GetType(const Texture::Ptr& ptr)
    {
        return ptr->GetType();
    }

    bool LocateFile(fs::path& aPath)
    {
        if (!aPath.has_filename())
            return false;

        if (fs::exists(aPath))
            return true;

        fs::path filename{ aPath.filename() };

        for (auto& entry : fs::recursive_directory_iterator{ fs::current_path() })
        {
            if (entry.is_regular_file() && entry.path().filename() == filename)
            {
                aPath = entry.path();
                return true;
            }
        }

        return false;
    }
}

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

        aiString aiPath{};
        if (aMaterial.GetTexture(aiType, 0, &aiPath) != aiReturn_SUCCESS)
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
        case aiTextureType_AMBIENT_OCCLUSION:
            type = TextureType::Occlusion;
            break;
        default:
            Debug::Println(std::format(
                "Warning: Unsupported texture type {}: {}",
                TextureTypeToString(aiType), aiPath.C_Str()
            ));
            continue;
        }

        fs::path path{ aiPath.C_Str() };

        if (!LocateFile(path))
        {
            Debug::Println(std::format(
                "Error: Could not locate texture: {}",
                aiPath.C_Str()
            ));

            continue;
        }

        if (auto texture = TextureFactory::Get().GetAsset(path, type))
            SetTexture(texture);
    }

    for (TextureType type : GetMaterialTextureTypes())
    {
        if (HasTexture(type))
            continue;

        fs::path path{ myName + "_" + TextureTypeToString(type) + ".jpg" };

        if (!fs::exists(path))
        {
            path.replace_extension("png");
            if (!fs::exists(path))
                continue;
        }

        if (auto texture = TextureFactory::Get().GetAsset(path, type))
            SetTexture(texture);
    }
}

void Material::SetTexture(Texture::Ptr aTexture)
{
    if (!aTexture || !*aTexture)
        return;
    auto itr = std::ranges::find(myTextures, aTexture->GetType(), GetType);
    if (itr != myTextures.end())
        *itr = aTexture;
    else
        myTextures.push_back(aTexture);
}

Texture::Ptr Material::GetTexture(TextureType aType) const
{
    auto itr = std::ranges::find(myTextures, aType, GetType);
    return itr != myTextures.end() ? *itr : nullptr;
}

bool Material::HasTexture(TextureType aType) const
{
    return std::ranges::contains(myTextures, aType, GetType);
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
            Image(texture->GetResource().Get(), { 128.f, 128.f });
            EndTooltip();
        }
    }
}
