#include "pch.h"
#include "GeometryBuffer.h"

GeometryBuffer::GeometryBuffer(unsigned aWidth, unsigned aHeight)
	: myTextures{}
	, myRenderTargets{}
	, myShaderResources{}
	, myResult{ S_OK }
{
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = aWidth;
	textureDesc.Height = aHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	D3D11_RENDER_TARGET_VIEW_DESC targetDesc{};
	targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc{};
	resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceDesc.Texture2D.MipLevels = static_cast<UINT>(-1);

	for (size_t i = 0; i < ourFormats.size(); ++i)
	{
		textureDesc.Format = targetDesc.Format = resourceDesc.Format = ourFormats[i];

		auto& texture = myTextures[i];
		auto& target = myRenderTargets[i];
		auto& resource = myShaderResources[i];

		myResult = DX11_DEVICE->CreateTexture2D(&textureDesc, NULL, &texture);
		if (FAILED(myResult))
			return;
		myResult = DX11_DEVICE->CreateRenderTargetView(texture.Get(), &targetDesc, &target);
		if (FAILED(myResult))
			return;
		myResult = DX11_DEVICE->CreateShaderResourceView(texture.Get(), &resourceDesc, &resource);
		if (FAILED(myResult))
			return;
	}
}
