#include "pch.h"
#include "SwapChain.h"

SwapChain::SwapChain(HWND hWnd)
	: myResult{}
	, mySwapChain{}
	, myBackBuffer{}
	, myRenderTarget{}
	, myWidth{}
	, myHeight{}
{
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

	D3D11_RENDER_TARGET_VIEW_DESC targetDesc{};
	targetDesc.Format = swapChainDesc.BufferDesc.Format;
	targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	ComPtr<IDXGIFactory> factory{};
	myResult = CreateDXGIFactory(IID_PPV_ARGS(&factory));
	if (FAILED(myResult))
		return;
	myResult = factory->CreateSwapChain(DX11_DEVICE, &swapChainDesc, &mySwapChain);
	if (FAILED(myResult))
		return;
	myResult = mySwapChain->GetBuffer(0, IID_PPV_ARGS(myBackBuffer.GetAddressOf()));
	if (FAILED(myResult))
		return;
	myResult = DX11_DEVICE->CreateRenderTargetView(myBackBuffer.Get(), &targetDesc, &myRenderTarget);
	if (FAILED(myResult))
		return;

	D3D11_TEXTURE2D_DESC desc{};
	myBackBuffer->GetDesc(&desc);
	myWidth = desc.Width;
	myHeight = desc.Height;
}

void SwapChain::ClearRenderTargets(const Color& aColor)
{
	DX11_CONTEXT->ClearRenderTargetView(myRenderTarget.Get(), aColor);
}

void SwapChain::Present() const
{
	if (mySwapChain)
		mySwapChain->Present(0, 0);
}

void SwapChain::GetDimensions(unsigned& aWidth, unsigned& aHeight) const
{
	aWidth = myWidth;
	aHeight = myHeight;
}
