#include "pch.h"
#include "Cubemap.h"
#include "Scopes.h"

Cubemap::Cubemap(std::span<const Image, 6> someFaces)
	: myResult{ E_FAIL }
	, myTexture{}
	, myShaderResource{}
	, myWidth{}, myHeight{}
{
	const unsigned width = someFaces[0].GetWidth();
	const unsigned height = someFaces[0].GetHeight();

	if (!std::ranges::all_of(someFaces, &Image::operator bool))
		return;
	if (std::ranges::count(someFaces, 4u, &Image::GetChannels) != 6)
		return;
	if (std::ranges::count(someFaces, width, &Image::GetWidth) != 6)
		return;
	if (std::ranges::count(someFaces, height, &Image::GetHeight) != 6)
		return;

	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc{};
	resourceDesc.Format = textureDesc.Format;
	resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	resourceDesc.TextureCube.MostDetailedMip = 0;
	resourceDesc.TextureCube.MipLevels = textureDesc.MipLevels;

	D3D11_SUBRESOURCE_DATA data[6]{};
	for (size_t i = 0; i < 6; ++i)
	{
		data[i].pSysMem = someFaces[i].GetData();
		data[i].SysMemPitch = width;
		data[i].SysMemSlicePitch = 0;
	}

	myResult = DX11_DEVICE->CreateTexture2D(&textureDesc, data, &myTexture);
	if (FAILED(myResult))
		return;
	myResult = DX11_DEVICE->CreateShaderResourceView(myTexture.Get(), &resourceDesc, &myShaderResource);
	if (FAILED(myResult))
		return;

	myWidth = width;
	myHeight = height;
}

void Cubemap::Draw() const
{
	if (!operator bool())
		return;

	CD3D11_RASTERIZER_DESC rasterizerDesc{ CD3D11_DEFAULT{} };
	rasterizerDesc.CullMode = D3D11_CULL_NONE;

	CD3D11_DEPTH_STENCIL_DESC depthStencilDesc{ CD3D11_DEFAULT{} };
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	
	ScopedPrimitiveTopology topology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };
	ScopedRasterizerState rasterizer{ rasterizerDesc };
	ScopedDepthStencilState depthStencil{ depthStencilDesc, 0 };

	// todo: draw
}
