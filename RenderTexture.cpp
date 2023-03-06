#include "pch.h"
#include "RenderTexture.h"

RenderTexture::RenderTexture(unsigned aWidth, unsigned aHeight, DXGI_FORMAT aFormat)
{
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = aWidth;
	textureDesc.Height = aHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = aFormat;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_RENDER_TARGET_VIEW_DESC targetDesc{};
	targetDesc.Format = aFormat;
	targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	targetDesc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc{};
	resourceDesc.Format = aFormat;
	resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceDesc.Texture2D.MostDetailedMip = 0;
	resourceDesc.Texture2D.MipLevels = static_cast<UINT>(-1);

	myResult = DX11_DEVICE->CreateTexture2D(&textureDesc, NULL, &myTexture);
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateRenderTargetView(myTexture.Get(), &targetDesc, &myRenderTarget);
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateShaderResourceView(myTexture.Get(), &resourceDesc, &myShaderResource);
	if (FAILED(myResult))
		return;

	myWidth = aWidth;
	myHeight = aHeight;
}

void RenderTexture::Clear(const Color& aColor)
{
	if (myRenderTarget)
		DX11_CONTEXT->ClearRenderTargetView(myRenderTarget.Get(), aColor);
}

RenderTexture::operator bool() const
{
	return SUCCEEDED(myResult);
}
