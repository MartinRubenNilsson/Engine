#include "pch.h"
#include "Texture.h"
#include "Image.h"

const char* TextureTypeToString(TextureType aType)
{
    static constexpr std::array strings
    {
        "Unknown",
        "Albedo",
        "Normal",
        "Metallic",
        "Roughness",
        "Occlusion",
        "HDR"
    };

    return strings.at(std::to_underlying(aType));
}

DXGI_FORMAT TextureTypeToFormat(TextureType aType)
{
	static constexpr std::array formats
	{
        DXGI_FORMAT_UNKNOWN,             // Unknown
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, // Albedo
		DXGI_FORMAT_R8G8B8A8_UNORM,		 // Normal
		DXGI_FORMAT_R8_UNORM,			 // Metallic
		DXGI_FORMAT_R8_UNORM,			 // Roughness
		DXGI_FORMAT_R8_UNORM,			 // Occlusion
        DXGI_FORMAT_R32G32B32A32_FLOAT,  // HDR
	};

	return formats.at(std::to_underlying(aType));
}

unsigned TextureTypeToChannels(TextureType aType)
{
	static constexpr std::array channels
	{
        0u, // Unknown
		4u, // Albedo
		4u, // Normal
		1u, // Metallic
		1u, // Roughness
		1u, // Occlusion
        4u, // HDR
	};

	return channels.at(std::to_underlying(aType));
}

/*
* class Texture
*/

Texture::Texture(const fs::path& aPath, TextureType aType)
{
    if (aType == TextureType::Unknown)
        return;

	Image image{ aPath, TextureTypeToChannels(aType) };
	if (!image)
		return;

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = image.GetWidth();
    desc.Height = image.GetHeight();
    desc.MipLevels = 0;
    desc.ArraySize = 1;
    desc.Format = TextureTypeToFormat(aType);
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    myResult = DX11_DEVICE->CreateTexture2D(&desc, NULL, &myTexture);
    if (FAILED(myResult))
        return;

    DX11_CONTEXT->UpdateSubresource(myTexture.Get(), 0, NULL, image.Data(), image.GetRowPitch(), 0);

    myResult = DX11_DEVICE->CreateShaderResourceView(myTexture.Get(), NULL, &myShaderResource);
    if (FAILED(myResult))
        return;

    DX11_CONTEXT->GenerateMips(myShaderResource.Get());

    myPath = aPath;
    myType = aType;
    myWidth = image.GetWidth();
    myHeight = image.GetHeight();
}

Texture::operator bool() const
{
	return SUCCEEDED(myResult);
}

void to_json(json& j, const Texture::Ptr& t)
{
    j["path"] = t->myPath;
    j["type"] = t->myType;
}

void from_json(const json& j, Texture::Ptr& t)
{
    t = TextureFactory::Get().GetAsset(
        j.at("path"),
        j.at("type").get<TextureType>()
    );
}
