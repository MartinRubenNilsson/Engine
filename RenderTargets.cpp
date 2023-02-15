#include "pch.h"
#include "RenderTargets.h"

RenderTargets::RenderTargets(unsigned aWidth, unsigned aHeight, std::span<const DXGI_FORMAT> someFormats)
	: myWidth{ aWidth }, myHeight{ aHeight } 
{
	if (someFormats.size() > D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)
		return;

	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = aWidth;
	textureDesc.Height = aHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_RENDER_TARGET_VIEW_DESC targetDesc{};
	targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc{};
	resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceDesc.Texture2D.MipLevels = static_cast<UINT>(-1);

	for (DXGI_FORMAT format : someFormats)
	{
		textureDesc.Format = targetDesc.Format = resourceDesc.Format = format;

		TexturePtr texture{};
		RenderTargetPtr target{};
		ShaderResourcePtr resource{};

		myResult = DX11_DEVICE->CreateTexture2D(&textureDesc, NULL, &texture);
		if (FAILED(myResult))
			return;
		myResult = DX11_DEVICE->CreateRenderTargetView(texture.Get(), &targetDesc, &target);
		if (FAILED(myResult))
			return;
		myResult = DX11_DEVICE->CreateShaderResourceView(texture.Get(), &resourceDesc, &resource);
		if (FAILED(myResult))
			return;

		myTextures.push_back(texture);
		myRenderTargets.push_back(target);
		myShaderResources.push_back(resource);
	}
}

void RenderTargets::Clear()
{
	static constexpr FLOAT color[]{ 0.f, 0.f, 0.f, 0.f };

	for (const auto& target : myRenderTargets)
		DX11_CONTEXT->ClearRenderTargetView(target.Get(), color);
}

RenderTargets::operator bool() const
{
	return SUCCEEDED(myResult);
}
