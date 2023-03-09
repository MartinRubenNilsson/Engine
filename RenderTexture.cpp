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

	myResult = DX11_DEVICE->CreateTexture2D(&textureDesc, NULL, &myTexture);
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateRenderTargetView(myTexture.Get(), &targetDesc, &myRenderTarget);
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateShaderResourceView(myTexture.Get(), NULL, &myShaderResource);
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

Viewport RenderTexture::GetViewport() const
{
	return Viewport{ CD3D11_VIEWPORT{ myTexture.Get(), myRenderTarget.Get() } };
}

RenderTexture::operator bool() const
{
	return SUCCEEDED(myResult);
}
