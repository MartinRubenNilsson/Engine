#include "pch.h"
#include "Cubemap.h"
#include "Image.h"
#include "Scopes.h"

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
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
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

	myResult = DX11_DEVICE->CreateShaderResourceView(texture.Get(), &resourceDesc, &myShaderResource);
	if (FAILED(myResult))
		return;
}

Cubemap::Cubemap(const fs::path& anEquirectHdrImage)
{
	ShaderResourcePtr equirectResource{}; // We will render from this resource...
	TexturePtr cubemapTexture{};
	RenderTargetPtr cubemapTarget{}; // ...to this target.

	// 1. Load equirectangular map and create corresponding shader resource.
	{
		Image image{ anEquirectHdrImage, 4 };
		if (!image || !image.IsHdr())
			return;

		D3D11_TEXTURE2D_DESC textureDesc{};
		textureDesc.Width = image.GetWidth();
		textureDesc.Height = image.GetHeight();
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA textureData{};
		textureData.pSysMem = image.Data();
		textureData.SysMemPitch = image.GetChannels() * image.GetWidth();
		textureData.SysMemSlicePitch = 0;

		D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc{};
		resourceDesc.Format = textureDesc.Format;
		resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		resourceDesc.Texture2D.MostDetailedMip = 0;
		resourceDesc.Texture2D.MipLevels = textureDesc.MipLevels;

		TexturePtr texture{};
		myResult = DX11_DEVICE->CreateTexture2D(&textureDesc, &textureData, &texture);
		if (FAILED(myResult))
			return;

		myResult = DX11_DEVICE->CreateShaderResourceView(texture.Get(), &resourceDesc, &equirectResource);
		if (FAILED(myResult))
			return;
	}
	
	// 2. Create cubemap shader resource and render target.
	{
		D3D11_TEXTURE2D_DESC textureDesc{};
		textureDesc.Width = 512; // HARDCODED OH NO!
		textureDesc.Height = 512; // HARDCODED OH NO!
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

		myResult = DX11_DEVICE->CreateTexture2D(&textureDesc, NULL, &cubemapTexture);
		if (FAILED(myResult))
			return;

		myResult = DX11_DEVICE->CreateShaderResourceView(cubemapTexture.Get(), &resourceDesc, &myShaderResource);
		if (FAILED(myResult))
			return;

		myResult = DX11_DEVICE->CreateRenderTargetView(cubemapTexture.Get(), &targetDesc, &cubemapTarget);
		if (FAILED(myResult))
			return;
	}

	// 3. Project (render) equirectangular map onto cubemap.
	ScopedPrimitiveTopology scopedTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	ScopedInputLayout scopedLayout{ typeid(EmptyVertex) };
	ScopedShader scopedVs{ VERTEX_SHADER("VsFullscreenTriangle.cso") };
	ScopedShader scopedGs{ GEOMETRY_SHADER("GsCubemap.cso") };
	ScopedShader scopedPs{ PIXEL_SHADER("PsCubemapTest.cso") };
	ScopedRenderTargets scopedTargets{ cubemapTarget };
	ScopedViewports scopedViewports{ CD3D11_VIEWPORT{ cubemapTexture.Get(), cubemapTarget.Get() } };
	DX11_CONTEXT->Draw(3, 0);
}

void Cubemap::DrawSkybox() const
{
	if (!operator bool())
		return;

	CD3D11_RASTERIZER_DESC rasterizerDesc{ CD3D11_DEFAULT{} };
	rasterizerDesc.CullMode = D3D11_CULL_FRONT; // Since the skybox surrounds us

	CD3D11_DEPTH_STENCIL_DESC depthStencilDesc{ CD3D11_DEFAULT{} };
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Otherwise z=1 will fail the depth test
	
	ScopedPrimitiveTopology scopedTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };
	ScopedInputLayout scopedLayout{ typeid(EmptyVertex) };
	ScopedShaderResources scopedResources{ ShaderType::Pixel, 0, myShaderResource };
	ScopedShader scopedVs{ VERTEX_SHADER("VsSkybox.cso") };
	ScopedShader scopedPs{ PIXEL_SHADER("PsSkybox.cso") };
	ScopedRasterizerState scopedRasterizer{ rasterizerDesc };
	ScopedDepthStencilState scopedDepthStencil{ depthStencilDesc };

	DX11_CONTEXT->Draw(14, 0);
}

Cubemap::operator bool() const
{
	return SUCCEEDED(myResult);
}
