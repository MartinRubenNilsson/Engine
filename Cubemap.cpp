#include "pch.h"
#include "Cubemap.h"
#include "Image.h"
#include "Scopes.h"
#include "ConstantBuffer.h"
#include "ShaderCommon.h"

namespace
{
	using namespace DirectX;

	const XMVECTOR theOrigin{ 0.f, 0.f, 0.f, 1.f };
	const XMVECTOR theX{ 1.f, 0.f, 0.f, 0.f };
	const XMVECTOR theY{ 0.f, 1.f, 0.f, 0.f };
	const XMVECTOR theZ{ 0.f, 0.f, 1.f, 0.f };
	const XMMATRIX theProj{ XMMatrixPerspectiveFovLH(XM_PIDIV2, 1.f, 0.1f, 10.f) };

	const std::array<XMMATRIX, 6> theCubeFaceViewProjs
	{
		XMMatrixLookToLH(theOrigin, +theX, +theY) * theProj, // +X
		XMMatrixLookToLH(theOrigin, -theX, +theY) * theProj, // -X
		XMMatrixLookToLH(theOrigin, +theY, -theZ) * theProj, // +Y
		XMMatrixLookToLH(theOrigin, -theY, +theZ) * theProj, // -Y
		XMMatrixLookToLH(theOrigin, +theZ, +theY) * theProj, // +Z
		XMMatrixLookToLH(theOrigin, -theZ, +theY) * theProj, // -Z
	};
}

Cubemap::Cubemap(std::span<const fs::path, 6> someLdrImages)
{
	std::array<Image, 6> images{};

	for (size_t i = 0; i < 6; ++i)
	{
		images[i] = { someLdrImages[i], 4 };
		if (!images[i])
			return;
	}

	const unsigned width = images[0].GetWidth();
	const unsigned height = images[0].GetHeight();

	if (std::ranges::count(images, width, &Image::GetWidth) != 6)
		return;
	if (std::ranges::count(images, height, &Image::GetHeight) != 6)
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
		textureData[i].pSysMem = images[i].Data();
		textureData[i].SysMemPitch = width * 4;
		textureData[i].SysMemSlicePitch = 0;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc{};
	resourceDesc.Format = textureDesc.Format;
	resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	resourceDesc.TextureCube.MostDetailedMip = 0;
	resourceDesc.TextureCube.MipLevels = textureDesc.MipLevels;

	D3D11_RENDER_TARGET_VIEW_DESC targetDesc{};
	targetDesc.Format = textureDesc.Format;
	targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	targetDesc.Texture2DArray.MipSlice = 0;
	targetDesc.Texture2DArray.FirstArraySlice = 0;
	targetDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;

	TexturePtr texture{};
	myResult = DX11_DEVICE->CreateTexture2D(&textureDesc, textureData, &texture);
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateShaderResourceView(texture.Get(), &resourceDesc, &myEnvironmentMap);
	if (FAILED(myResult))
		return;

	CreateIrradianceMap();
}

//Cubemap::Cubemap(const fs::path& anEquirectHdrImage)
//{
//	ShaderResourcePtr equirectResource{}; // We will render from this resource...
//	TexturePtr cubemapTexture{};
//	RenderTargetPtr cubemapTarget{}; // ...to this target.
//
//	// 1. Load equirectangular map and create corresponding shader resource.
//	{
//		Image image{ anEquirectHdrImage, 4 };
//		if (!image || !image.IsHdr())
//			return;
//
//		D3D11_TEXTURE2D_DESC textureDesc{};
//		textureDesc.Width = image.GetWidth();
//		textureDesc.Height = image.GetHeight();
//		textureDesc.MipLevels = 1;
//		textureDesc.ArraySize = 1;
//		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//		textureDesc.SampleDesc.Count = 1;
//		textureDesc.SampleDesc.Quality = 0;
//		textureDesc.Usage = D3D11_USAGE_DEFAULT;
//		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
//		textureDesc.CPUAccessFlags = 0;
//		textureDesc.MiscFlags = 0;
//
//		D3D11_SUBRESOURCE_DATA textureData{};
//		textureData.pSysMem = image.Data();
//		textureData.SysMemPitch = sizeof(float) * image.GetChannels() * image.GetWidth();
//		textureData.SysMemSlicePitch = 0;
//
//		D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc{};
//		resourceDesc.Format = textureDesc.Format;
//		resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//		resourceDesc.Texture2D.MostDetailedMip = 0;
//		resourceDesc.Texture2D.MipLevels = textureDesc.MipLevels;
//		
//		TexturePtr requirectTexture{};
//
//		myResult = DX11_DEVICE->CreateTexture2D(&textureDesc, &textureData, &requirectTexture);
//		if (FAILED(myResult))
//			return;
//
//		myResult = DX11_DEVICE->CreateShaderResourceView(requirectTexture.Get(), &resourceDesc, &equirectResource);
//		if (FAILED(myResult))
//			return;
//	}
//	
//	// 2. Create cubemap shader resource and render target.
//	{
//		D3D11_TEXTURE2D_DESC textureDesc{};
//		textureDesc.Width = 512; // HARDCODED OH NO!
//		textureDesc.Height = 512; // HARDCODED OH NO!
//		textureDesc.MipLevels = 1;
//		textureDesc.ArraySize = 6;
//		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//		textureDesc.SampleDesc.Count = 1;
//		textureDesc.SampleDesc.Quality = 0;
//		textureDesc.Usage = D3D11_USAGE_DEFAULT;
//		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
//		textureDesc.CPUAccessFlags = 0;
//		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
//
//		D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc{};
//		resourceDesc.Format = textureDesc.Format;
//		resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
//		resourceDesc.TextureCube.MostDetailedMip = 0;
//		resourceDesc.TextureCube.MipLevels = textureDesc.MipLevels;
//
//		D3D11_RENDER_TARGET_VIEW_DESC targetDesc{};
//		targetDesc.Format = textureDesc.Format;
//		targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
//		targetDesc.Texture2DArray.MipSlice = 0;
//		targetDesc.Texture2DArray.FirstArraySlice = 0;
//		targetDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;
//
//		myResult = DX11_DEVICE->CreateTexture2D(&textureDesc, NULL, &cubemapTexture);
//		if (FAILED(myResult))
//			return;
//
//		myResult = DX11_DEVICE->CreateShaderResourceView(cubemapTexture.Get(), &resourceDesc, &myEnvironmentMap);
//		if (FAILED(myResult))
//			return;
//
//		myResult = DX11_DEVICE->CreateRenderTargetView(cubemapTexture.Get(), &targetDesc, &cubemapTarget);
//		if (FAILED(myResult))
//			return;
//	}
//
//	// 3. Project (render) equirectangular map onto cubemap.
//	ScopedPrimitiveTopology scopedTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
//	ScopedInputLayout scopedLayout{ typeid(EmptyVertex) };
//	ScopedShader scopedVs{ VERTEX_SHADER("VsGenerateCubemap.cso") };
//	ScopedShader scopedGs{ GEOMETRY_SHADER("GsGenerateCubemap.cso") };
//	ScopedShader scopedPs{ PIXEL_SHADER("PsGenerateCubemap.cso") };
//	ScopedSamplerStates scopedSampler{ 0, CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} } };
//	ScopedShaderResources scopedResource{ ShaderType::Pixel, 0, equirectResource };
//	ScopedRenderTargets scopedTarget{ cubemapTarget };
//	ScopedViewports scopedViewports{ CD3D11_VIEWPORT{ cubemapTexture.Get(), cubemapTarget.Get() } };
//	DX11_CONTEXT->Draw(3, 0);
//}

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

	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = size;
	textureDesc.Height = size;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc{};
	resourceDesc.Format = textureDesc.Format;
	resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	resourceDesc.TextureCube.MostDetailedMip = 0;
	resourceDesc.TextureCube.MipLevels = textureDesc.MipLevels;

	D3D11_RENDER_TARGET_VIEW_DESC targetDesc{};
	targetDesc.Format = textureDesc.Format;
	targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	targetDesc.Texture2DArray.MipSlice = 0;
	targetDesc.Texture2DArray.FirstArraySlice = 0;
	targetDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;

	TexturePtr irradianceTexture{};
	RenderTargetPtr irradianceTarget{};

	myResult = DX11_DEVICE->CreateTexture2D(&textureDesc, NULL, &irradianceTexture);
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateShaderResourceView(irradianceTexture.Get(), &resourceDesc, &myIrradianceMap);
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateRenderTargetView(irradianceTexture.Get(), &targetDesc, &irradianceTarget);
	if (FAILED(myResult))
		return;

	ScopedPrimitiveTopology scopedTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };
	ScopedInputLayout scopedLayout{ typeid(EmptyVertex) };
	ScopedShader scopedVs{ "VsCubemap.cso" };
	ScopedShader scopedGs{ "GsGenCubemap.cso" };
	ScopedShader scopedPs{ "PsGenIrradianceMap.cso" };
	ScopedShaderResources scopedResources{ ShaderType::Pixel, t_EnvironmentMap, myEnvironmentMap };
	ScopedSamplerStates scopedSamplers{ s_TrilinearSampler, CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} } };
	ScopedRenderTargets scopedTargets{ irradianceTarget };
	ScopedViewports scopedViewports{ CD3D11_VIEWPORT{ irradianceTexture.Get(), irradianceTarget.Get() } };

	ConstantBuffer cubeFaceViewProjs{ sizeof(theCubeFaceViewProjs) };
	cubeFaceViewProjs.Update(&theCubeFaceViewProjs);
	cubeFaceViewProjs.GSSetBuffer(b_Cubemap);

	DX11_CONTEXT->Draw(14, 0);
}
