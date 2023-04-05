#include "pch.h"
#include "DepthBuffer.h"
#include "DX11.h"

DepthBuffer::DepthBuffer(unsigned aWidth, unsigned aHeight)
{
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = aWidth;
	textureDesc.Height = aHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc{};
	depthDesc.Format = textureDesc.Format;
	depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthDesc.Texture2D.MipSlice = 0;

	myResult = DX11::GetDevice()->CreateTexture2D(&textureDesc, NULL, &myTexture);
	if (FAILED(myResult))
		return;

	myResult = DX11::GetDevice()->CreateDepthStencilView(myTexture.Get(), &depthDesc, &myDepthStencil);
	if (FAILED(myResult))
		return;

	myWidth = aWidth;
	myHeight = aHeight;
}

void DepthBuffer::Clear(float aDepth)
{
	if (myDepthStencil)
		DX11::GetContext()->ClearDepthStencilView(myDepthStencil.Get(), D3D11_CLEAR_DEPTH, aDepth, 0);
}

DepthBuffer::operator bool() const
{
	return SUCCEEDED(myResult);
}
