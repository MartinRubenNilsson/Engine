#include "pch.h"
#include "Material.h"
#include "Image.h"

const char* TextureTypeToString(TextureType aType)
{
    static constexpr std::array strings
    {
        "Albedo",
        "Normal",
        "Metallic",
        "Roughness",
        "Occlusion",
        //"Emissive",
    };

    return strings.at(std::to_underlying(aType));
}

/*
* class Material
*/

Material::Material(const aiMaterial& aMaterial)
    : myName{ aMaterial.GetName().C_Str() }
{
    LoadPaths(aMaterial);
    LoadImages();
    CreateTextures();
    CreateShaderResources();
}

const fs::path& Material::GetPath(TextureType aType) const
{
    return myPaths.at(std::to_underlying(aType));
}

ShaderResourcePtr Material::GetShaderResource(TextureType aType) const
{
    return myShaderResources.at(std::to_underlying(aType));
}

void Material::LoadPaths(const aiMaterial& aMaterial)
{
    for (int i = aiTextureType_DIFFUSE; i < aiTextureType_UNKNOWN; ++i)
    {
        aiTextureType aiType{ static_cast<aiTextureType>(i) };

        if (aMaterial.GetTextureCount(aiType) == 0)
            continue;

        aiString path{};
        if (aMaterial.GetTexture(aiType, 0, &path) != aiReturn_SUCCESS)
            continue;

        TextureType type;

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
        // todo: handle ambient occlusion
        case aiTextureType_EMISSIVE:
            [[fallthrough]];
        default:
            Debug::Println(std::format("Warning: Unrecognized texture type {}", TextureTypeToString(aiType)));
            continue;
        }

        myPaths[std::to_underlying(type)] = path.C_Str();
    }
}

void Material::LoadImages()
{
    for (size_t i = 0; i < ourCount; ++i)
    {
        if (!myPaths[i].empty())
            myImages[i] = { myPaths[i], ourChannels[i] };
    }
}

void Material::CreateTextures()
{
    for (size_t i = 0; i < ourCount; ++i)
    {
        if (!myImages[i])
            continue;

        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = myImages[i].GetWidth();
        desc.Height = myImages[i].GetHeight();
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = ourFormats[i];
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA data{};
        data.pSysMem = myImages[i].Data();
        data.SysMemPitch = desc.Width * myImages[i].GetChannels();
        data.SysMemSlicePitch = 0;

        DX11_DEVICE->CreateTexture2D(&desc, &data, &myTextures[i]);
    }
}

void Material::CreateShaderResources()
{
    for (size_t i = 0; i < ourCount; ++i)
    {
        if (myTextures[i])
            DX11_DEVICE->CreateShaderResourceView(myTextures[i].Get(), NULL, &myShaderResources[i]);
    }
}

/*
* ImGui
*/

void ImGui::InspectMaterial(const Material& aMaterial)
{
    Text(aMaterial.GetName().data());

    const float availWidth = GetContentRegionAvail().x;

    for (size_t i = 0; i < std::to_underlying(TextureType::Count); ++i)
    {
        TextureType type{ i };

        if (TreeNode(TextureTypeToString(type)))
        {
            if (aMaterial.GetPath(type).empty())
            {
                TextColored({ 1.f, 1.f, 0.f, 1.f }, "No texture specified!");
            }
            else
            {
                Text(aMaterial.GetPath(type).filename().string().c_str());

                if (ShaderResourcePtr resource = aMaterial.GetShaderResource(type))
                    Image(resource.Get(), { availWidth, availWidth });
                else
                    TextColored({ 1.f, 0.f, 0.f, 1.f }, "Failed to load texture!");
            }

            TreePop();
        }
    }
}
