#include "pch.h"
#include "SwapChain.h"

SwapChain::SwapChain(HWND hWnd)
{
	{
		ComPtr<IDXGIFactory> factory{};
		myResult = CreateDXGIFactory(IID_PPV_ARGS(&factory));
		if (FAILED(myResult))
			return;

		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		myResult = factory->CreateSwapChain(DX11_DEVICE, &swapChainDesc, &mySwapChain);
		if (FAILED(myResult))
			return;
	}

	{
		myResult = mySwapChain->GetBuffer(0, IID_PPV_ARGS(myTexture2d.GetAddressOf()));
		if (FAILED(myResult))
			return;
	}

	{
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc{};
		renderTargetDesc.Format = DXGI_FORMAT_UNKNOWN;
		renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		myResult = DX11_DEVICE->CreateRenderTargetView(myTexture2d.Get(), &renderTargetDesc, &myRenderTargetView);
		if (FAILED(myResult))
			return;
	}
}

void SwapChain::Present()
{
	if (mySwapChain)
		mySwapChain->Present(0, 0);
}

void SwapChain::SetRenderTarget(ID3D11DepthStencilView* aDepthStencil)
{
	DX11_CONTEXT->OMSetRenderTargets(1, myRenderTargetView.GetAddressOf(), aDepthStencil);
}

void SwapChain::ClearRenderTarget(const float aColor[4])
{
	DX11_CONTEXT->ClearRenderTargetView(myRenderTargetView.Get(), aColor);
}

unsigned SwapChain::GetWidth() const
{
	D3D11_TEXTURE2D_DESC desc{};
	if (myTexture2d)
		myTexture2d->GetDesc(&desc);
	return desc.Width;
}

unsigned SwapChain::GetHeight() const
{
	D3D11_TEXTURE2D_DESC desc{};
	if (myTexture2d)
		myTexture2d->GetDesc(&desc);
	return desc.Height;
}
