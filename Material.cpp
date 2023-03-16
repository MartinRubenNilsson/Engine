#include "pch.h"
#include "Material.h"

/*
* class Material
*/

Material::Material(const aiMaterial& aMaterial)
    : myName{ aMaterial.GetName().C_Str() }
{
    auto& factory{ TextureFactory::Get() };

    for (int i = aiTextureType_DIFFUSE; i < aiTextureType_UNKNOWN; ++i)
    {
        aiTextureType aiType{ static_cast<aiTextureType>(i) };

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
            Debug::Println(std::format("Warning: Unrecognized texture type {}", TextureTypeToString(aiType)));
            continue;
        }

        if (Texture::Ptr texture{ factory.GetAsset(path.C_Str(), type) })
            myTextures.push_back(texture);
    }
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
    Text(aMaterial.GetName().data());

    for (const Texture::Ptr& texture : aMaterial.GetTextures())
    {
        if (texture && TreeNode(TextureTypeToString(texture->GetType())))
        {
            TextWrapped(texture->GetPath().string().c_str());
            Image(texture->GetResource().Get(), { 200.f, 200.f });
            TreePop();
        }
    }
}
