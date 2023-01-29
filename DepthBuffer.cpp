#include "pch.h"
#include "DepthBuffer.h"

DepthBuffer::DepthBuffer(unsigned aWidth, unsigned aHeight)
{
	{
		D3D11_TEXTURE2D_DESC textureDesc{};
		textureDesc.Width = aWidth;
		textureDesc.Height = aHeight;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		myResult = DX11_DEVICE->CreateTexture2D(&textureDesc, NULL, &myTexture2d);
		if (FAILED(myResult))
			return;
	}
	
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc{};
		depthDesc.Format = DXGI_FORMAT_UNKNOWN;
		depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		myResult = DX11_DEVICE->CreateDepthStencilView(myTexture2d.Get(), &depthDesc, &myDepthStencilView);
		if (FAILED(myResult))
			return;
	}
}

void DepthBuffer::ClearDepthStencil()
{
	DX11_CONTEXT->ClearDepthStencilView(myDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.f, 0);
}
