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

void SwapChain::ClearRenderTarget(const Color& aColor)
{
	DX11_CONTEXT->ClearRenderTargetView(myRenderTarget.Get(), aColor.operator const float* ());
}

void SwapChain::SetRenderTarget(ID3D11DepthStencilView* aDepthStencil) const
{
	DX11_CONTEXT->OMSetRenderTargets(1, myRenderTarget.GetAddressOf(), aDepthStencil);

	Viewport viewport{};
	viewport.width = static_cast<float>(myWidth);
	viewport.height = static_cast<float>(myHeight);

	DX11_CONTEXT->RSSetViewports(1, viewport.Get11());
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
