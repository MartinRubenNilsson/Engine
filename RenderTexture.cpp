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

void RenderTexture::GetTexel(std::span<std::byte> aBuffer, unsigned x, unsigned y)
{
	if (x >= myWidth || y >= myHeight)
		return;

	if (!myTexel && !CreateTexel())
		return;

	D3D11_BOX box{};
	box.left = x;
	box.right = x + 1;
	box.top = y;
	box.bottom = y + 1;
	box.front = 0;
	box.back = 1;

	DX11_CONTEXT->CopySubresourceRegion(myTexel.Get(), 0, 0, 0, 0, myTexture.Get(), 0, &box);

	D3D11_MAPPED_SUBRESOURCE resource{};
	DX11_CONTEXT->Map(myTexel.Get(), 0, D3D11_MAP_READ, 0, &resource);
	std::memcpy(aBuffer.data(), resource.pData, aBuffer.size_bytes());
	DX11_CONTEXT->Unmap(myTexel.Get(), 0);
}

Viewport RenderTexture::GetViewport() const
{
	return Viewport{ CD3D11_VIEWPORT{ myTexture.Get(), myRenderTarget.Get() } };
}

RenderTexture::operator bool() const
{
	return SUCCEEDED(myResult);
}

bool RenderTexture::CreateTexel()
{
	D3D11_TEXTURE2D_DESC desc{};
	myTexture->GetDesc(&desc);
	desc.Width = 1;
	desc.Height = 1;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	return SUCCEEDED(DX11_DEVICE->CreateTexture2D(&desc, NULL, &myTexel));
}
