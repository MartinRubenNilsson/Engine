#include "pch.h"
#include "SwapChain.h"

SwapChain::SwapChain(const Window& aWindow)
{
	{
		ComPtr<IDXGIFactory> factory{};
		if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(&factory))))
			return;

		DXGI_SWAP_CHAIN_DESC desc{};
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 2;
		desc.OutputWindow = aWindow.GetHandle();
		desc.Windowed = TRUE;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		if (FAILED(factory->CreateSwapChain(DX11_DEVICE, &desc, &mySwapChain)))
			return;
	}

	{
		ComPtr<ID3D11Resource> resource{};
		if (FAILED(mySwapChain->GetBuffer(0, IID_PPV_ARGS(&resource))))
			return;

		D3D11_RENDER_TARGET_VIEW_DESC desc{};
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		if (FAILED(DX11_DEVICE->CreateRenderTargetView(resource.Get(), &desc, &myRenderTargetView)))
			return;
	}
}

void SwapChain::Present()
{
	if (mySwapChain)
		mySwapChain->Present(0, 0);
}
