#include "pch.h"
#include "Cubemap.h"
#include "Image.h"
#include "Scopes.h"
#include "ShaderCommon.h"
#include "Texture.h"

Cubemap::Cubemap(std::span<const fs::path, 6> someLdrCubeFaces)
{
	std::array<Image, 6> faces{};

	for (size_t i = 0; i < 6; ++i)
	{
		faces[i] = { someLdrCubeFaces[i], 4 };
		if (!faces[i])
			return;
	}

	const unsigned width = faces[0].GetWidth();
	const unsigned height = faces[0].GetHeight();

	if (std::ranges::count(faces, width, &Image::GetWidth) != 6)
		return;
	if (std::ranges::count(faces, height, &Image::GetHeight) != 6)
		return;

	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	D3D11_SUBRESOURCE_DATA textureData[6]{};
	for (size_t i = 0; i < 6; ++i)
	{
		textureData[i].pSysMem = faces[i].Data();
		textureData[i].SysMemPitch = width * 4;
		textureData[i].SysMemSlicePitch = 0;
	}

	TexturePtr texture{};

	myResult = DX11_DEVICE->CreateTexture2D(&textureDesc, NULL, &texture);
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateShaderResourceView(texture.Get(), NULL, &myEnvironmentMap);
	if (FAILED(myResult))
		return;

	CreateIrradianceMap();
	CreatePrefilteredMap();
}

Cubemap::Cubemap(const fs::path& anHdrEquirectMap)
{
	static constexpr UINT size{ 512 };

	// 1. Load equirectangular map.
	Texture::Ptr equirectMap{ TextureFactory::Get().GetAsset(anHdrEquirectMap, TextureType::HDR) };
	if (!equirectMap)
		return;

	// 2. Create cubemap shader resource and render target.
	TexturePtr cubemapTexture{};
	RenderTargetPtr cubemapTarget{};
	{
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = size;
		desc.Height = size;
		desc.MipLevels = 1;
		desc.ArraySize = 6;
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		myResult = DX11_DEVICE->CreateTexture2D(&desc, NULL, &cubemapTexture);
		if (FAILED(myResult))
			return;

		myResult = DX11_DEVICE->CreateShaderResourceView(cubemapTexture.Get(), NULL, &myEnvironmentMap);
		if (FAILED(myResult))
			return;

		myResult = DX11_DEVICE->CreateRenderTargetView(cubemapTexture.Get(), NULL, &cubemapTarget);
		if (FAILED(myResult))
			return;
	}

	// 3. Project (render) equirectangular map onto cubemap.
	{
		ScopedPrimitiveTopology scopedTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };
		ScopedInputLayout scopedLayout{ typeid(EmptyVertex) };
		ScopedShader scopedVs{ "VsCubemap.cso" };
		ScopedShader scopedGs{ "GsGenCubemap.cso" };
		ScopedShader scopedPs{ "PsEquirectToCubemap.cso" };
		ScopedShaderResources scopedResource{ ShaderType::Pixel, 0, equirectMap->GetResource() };
		ScopedRenderTargets scopedTarget{ cubemapTarget };
		ScopedViewports scopedViewports{ CD3D11_VIEWPORT{ cubemapTexture.Get(), cubemapTarget.Get() } };

		DX11_CONTEXT->Draw(CUBEMAP_VERTEX_COUNT, 0);
	}
	
	CreateIrradianceMap();
	CreatePrefilteredMap();
}

std::vector<ShaderResourcePtr> Cubemap::GetMaps() const
{
	return { myEnvironmentMap, myIrradianceMap, myPrefilteredMap };
}

Cubemap::operator bool() const
{
	return SUCCEEDED(myResult);
}

void Cubemap::CreateIrradianceMap()
{
	static constexpr UINT size{ 32 };

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = size;
	desc.Height = size;
	desc.MipLevels = 1;
	desc.ArraySize = 6;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	TexturePtr irradianceTexture{};
	RenderTargetPtr irradianceTarget{};

	myResult = DX11_DEVICE->CreateTexture2D(&desc, NULL, &irradianceTexture);
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateShaderResourceView(irradianceTexture.Get(), NULL, &myIrradianceMap);
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateRenderTargetView(irradianceTexture.Get(), NULL, &irradianceTarget);
	if (FAILED(myResult))
		return;

	ScopedPrimitiveTopology scopedTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };
	ScopedInputLayout scopedLayout{ typeid(EmptyVertex) };
	ScopedShader scopedVs{ "VsCubemap.cso" };
	ScopedShader scopedGs{ "GsGenCubemap.cso" };
	ScopedShader scopedPs{ "PsGenIrradianceMap.cso" };
	ScopedShaderResources scopedResources{ ShaderType::Pixel, t_EnvironmentMap, myEnvironmentMap };
	ScopedRenderTargets scopedTargets{ irradianceTarget };
	ScopedViewports scopedViewports{ CD3D11_VIEWPORT{ irradianceTexture.Get(), irradianceTarget.Get() } };

	DX11_CONTEXT->Draw(CUBEMAP_VERTEX_COUNT, 0);
}

void Cubemap::CreatePrefilteredMap()
{
	 // todo
}
