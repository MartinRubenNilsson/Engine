#include "pch.h"
#include "Material.h"
#include "Image.h"

Material::Material(const aiMaterial& aMaterial)
    : myName{ aMaterial.GetName().C_Str() }
{
    std::vector<std::pair<aiTextureType, aiString>> textures{};

    for (aiTextureType type = aiTextureType_DIFFUSE; type < aiTextureType_UNKNOWN; type = static_cast<aiTextureType>(type + 1))
    {
        for (unsigned index = 0; index < aMaterial.GetTextureCount(type); ++index)
        {
            aiString path{};
            if (aMaterial.GetTexture(type, index, &path) == aiReturn_SUCCESS)
                textures.emplace_back(type, path);
        }
    }

    for (auto& [type, path] : textures)
    {
        Image image{ path.C_Str(), 4 };
        if (!image)
            continue;

        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = image.GetWidth();
        desc.Height = image.GetHeight();
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        if (type == aiTextureType_DIFFUSE)
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

        D3D11_SUBRESOURCE_DATA data{};
        data.pSysMem = image.Data();
        data.SysMemPitch = desc.Width * 4;
        data.SysMemSlicePitch = 0;

        // todo
        desc;
        data;
    }

    Debug::Println("Fuck");
}

void Material::CreateShaderResources()
{
    for (size_t i = 0; i < Count; ++i)
    {
        if (myTextures[i])
            DX11_DEVICE->CreateShaderResourceView(myTextures[i].Get(), NULL, &myShaderResources[i]);
    }
}
