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

        myTexturePaths[std::to_underlying(type)] = path.C_Str();
    }

    LoadAndCreateTexturesAndResources();
}

const fs::path& Material::GetPath(TextureType aType) const
{
    return myTexturePaths.at(std::to_underlying(aType));
}

ShaderResourcePtr Material::GetShaderResource(TextureType aType) const
{
    return myShaderResources.at(std::to_underlying(aType));
}

void Material::LoadAndCreateTexturesAndResources()
{
    for (size_t i = 0; i < ourCount; ++i)
    {
        if (myTexturePaths[i].empty())
            continue;

        Image image{ myTexturePaths[i], ourChannels[i] };
        if (!image)
            continue;

        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = image.GetWidth();
        desc.Height = image.GetHeight();
        desc.MipLevels = 0;
        desc.ArraySize = 1;
        desc.Format = ourFormats[i];
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

        DX11_DEVICE->CreateTexture2D(&desc, NULL, &myTextures[i]);
        if (!myTextures[i])
            continue;

        DX11_CONTEXT->UpdateSubresource(
            myTextures[i].Get(), 0, NULL,
            image.Data(), image.GetWidth() * image.GetChannels(), 0
        );

        DX11_DEVICE->CreateShaderResourceView(myTextures[i].Get(), NULL, &myShaderResources[i]);
        if (!myShaderResources[i])
            continue;

        DX11_CONTEXT->GenerateMips(myShaderResources[i].Get());
    }
}

/*
* namespace ImGui
*/

void ImGui::InspectMaterial(const Material& aMaterial)
{
    Text("Name: %s", aMaterial.GetName().data());

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
                Text("Path: %s", aMaterial.GetPath(type).filename().string().c_str());

                if (ShaderResourcePtr resource = aMaterial.GetShaderResource(type))
                    Image(resource.Get(), { 200.f, 200.f });
                else
                    TextColored({ 1.f, 0.f, 0.f, 1.f }, "Failed to load texture!");
            }

            TreePop();
        }
    }
}
