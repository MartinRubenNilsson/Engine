#include "pch.h"
#include "Cubemap.h"
#include "Scopes.h"
#include "InputLayoutManager.h"

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
		data[i].pSysMem = someFaces[i].Data();
		data[i].SysMemPitch = width * 4;
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

void Cubemap::DrawSkybox() const
{
	if (!operator bool())
		return;

	InputLayoutManager::Get().SetInputLayout(typeid(EmptyVertex));

	CD3D11_RASTERIZER_DESC rasterizerDesc{ CD3D11_DEFAULT{} };
	rasterizerDesc.CullMode = D3D11_CULL_FRONT; // Since the skybox surrounds us

	CD3D11_DEPTH_STENCIL_DESC depthStencilDesc{ CD3D11_DEFAULT{} };
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Otherwise z=1 will fail the depth test
	
	ScopedPrimitiveTopology topology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };
	ScopedShader vertexShader{ VERTEX_SHADER("VsSkybox.cso") };
	ScopedShader pixelShader{ PIXEL_SHADER("PsSkybox.cso") };
	ScopedRasterizerState rasterizer{ rasterizerDesc };
	ScopedDepthStencilState depthStencil{ depthStencilDesc, 0 };
	ScopedPixelShaderResources resources{ 0, *this };

	DX11_CONTEXT->Draw(14, 0);
}

Cubemap::operator std::span<ID3D11ShaderResourceView* const>() const
{
	return { myShaderResource.GetAddressOf(), 1 };
}
