#include "pch.h"
#include "Cubemap.h"
#include "Image.h"
#include "Scopes.h"

Cubemap::Cubemap(std::span<const fs::path, 6> someImagePaths)
{
	std::array<Image, 6> images{};

	for (size_t i = 0; i < 6; ++i)
	{
		images[i] = { someImagePaths[i], 4 };
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
	// textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	D3D11_SUBRESOURCE_DATA data[6]{};
	for (size_t i = 0; i < 6; ++i)
	{
		data[i].pSysMem = images[i].Data();
		data[i].SysMemPitch = width * 4;
		data[i].SysMemSlicePitch = 0;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc{};
	resourceDesc.Format = textureDesc.Format;
	resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	resourceDesc.TextureCube.MostDetailedMip = 0;
	resourceDesc.TextureCube.MipLevels = textureDesc.MipLevels;

	/*D3D11_RENDER_TARGET_VIEW_DESC targetDesc{};
	targetDesc.Format = textureDesc.Format;
	targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	targetDesc.Texture2DArray.MipSlice = 0;
	targetDesc.Texture2DArray.FirstArraySlice = 0;
	targetDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;*/

	myResult = DX11_DEVICE->CreateTexture2D(&textureDesc, data, &myTexture);
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateShaderResourceView(myTexture.Get(), &resourceDesc, &myShaderResource);
	if (FAILED(myResult))
		return;

	/*myResult = DX11_DEVICE->CreateRenderTargetView(myTexture.Get(), &targetDesc, &myRenderTarget);
	if (FAILED(myResult))
		return;*/

	myWidth = width;
	myHeight = height;


	///*
	//* TESTING
	//*/

	//ScopedRenderTargets scopedTargets{ myRenderTarget };
	//ScopedShaderResources scopedResources{ SHader}
}

void Cubemap::DrawSkybox() const
{
	if (!operator bool())
		return;

	CD3D11_RASTERIZER_DESC rasterizerDesc{ CD3D11_DEFAULT{} };
	rasterizerDesc.CullMode = D3D11_CULL_FRONT; // Since the skybox surrounds us

	CD3D11_DEPTH_STENCIL_DESC depthStencilDesc{ CD3D11_DEFAULT{} };
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Otherwise z=1 will fail the depth test
	
	ScopedPrimitiveTopology topology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };
	ScopedInputLayout layout{ typeid(EmptyVertex) };
	ScopedShaderResources resources{ ShaderType::Pixel, 0, *this };
	ScopedShader vertexShader{ VERTEX_SHADER("VsSkybox.cso") };
	ScopedShader pixelShader{ PIXEL_SHADER("PsSkybox.cso") };
	ScopedRasterizerState rasterizer{ rasterizerDesc };
	ScopedDepthStencilState depthStencil{ depthStencilDesc };

	DX11_CONTEXT->Draw(14, 0);
}

Cubemap::operator bool() const
{
	return SUCCEEDED(myResult);
}

Cubemap::operator std::span<const ShaderResourcePtr>() const
{
	return { std::addressof(myShaderResource), 1 };
}
