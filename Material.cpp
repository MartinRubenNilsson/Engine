#include "pch.h"
#include "Material.h"
#include "Image.h"

Material::Material(const aiMaterial& aMaterial)
    : myName{ aMaterial.GetName().C_Str() }
{
    LoadPaths(aMaterial);
    LoadImages();
    CreateTextures();
    CreateShaderResources();
}

void Material::LoadPaths(const aiMaterial& aMaterial)
{
    for (aiTextureType aiType = aiTextureType_DIFFUSE; aiType < aiTextureType_UNKNOWN; aiType = static_cast<aiTextureType>(aiType + 1))
    {
        if (aMaterial.GetTextureCount(aiType) == 0)
            continue;

        aiString path{};
        if (aMaterial.GetTexture(aiType, 0, &path) != aiReturn_SUCCESS)
            continue;

        TextureType type;

        switch (aiType)
        {
        case aiTextureType_DIFFUSE:
            type = Diffuse;
            break;
        case aiTextureType_EMISSIVE:
            type = Emissive;
            break;
        case aiTextureType_NORMALS:
            type = Normal;
            break;
        case aiTextureType_METALNESS:
            type = Metallic;
            break;
        case aiTextureType_SHININESS:
            type = Roughness;
            break;
        default:
            Debug::Println(std::format("Warning: Unrecognized texture type {}", TextureTypeToString(aiType)));
            continue;
        }

        myPaths[type] = path.C_Str();
    }
}

void Material::LoadImages()
{
    for (size_t type = 0; type < Count; ++type)
    {
        if (!myPaths[type].empty())
            myImages[type] = { myPaths[type], ourChannels[type] };
    }
}

void Material::CreateTextures()
{
    for (size_t type = 0; type < Count; ++type)
    {
        if (!myImages[type])
            continue;

        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = myImages[type].GetWidth();
        desc.Height = myImages[type].GetHeight();
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = ourFormats[type];
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA data{};
        data.pSysMem = myImages[type].Data();
        data.SysMemPitch = desc.Width * ourChannels[type];
        data.SysMemSlicePitch = 0;

        DX11_DEVICE->CreateTexture2D(&desc, &data, &myTextures[type]);
    }
}

void Material::CreateShaderResources()
{
    for (size_t type = 0; type < Count; ++type)
    {
        if (myTextures[type])
            DX11_DEVICE->CreateShaderResourceView(myTextures[type].Get(), NULL, &myShaderResources[type]);
    }
}
