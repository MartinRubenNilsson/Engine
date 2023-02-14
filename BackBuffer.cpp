#include "pch.h"
#include "BackBuffer.h"

BackBuffer::BackBuffer(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = 0; // Use window width
	swapChainDesc.BufferDesc.Height = 0; // Use window height
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	D3D11_RENDER_TARGET_VIEW_DESC targetDesc{};
	targetDesc.Format = swapChainDesc.BufferDesc.Format;
	targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	targetDesc.Texture2D.MipSlice = 0;

	ComPtr<IDXGIFactory> factory{};
	myResult = CreateDXGIFactory(IID_PPV_ARGS(&factory));
	if (FAILED(myResult))
		return;
	myResult = factory->CreateSwapChain(DX11_DEVICE, &swapChainDesc, &mySwapChain);
	if (FAILED(myResult))
		return;
	myResult = mySwapChain->GetBuffer(0, IID_PPV_ARGS(myTexture.GetAddressOf()));
	if (FAILED(myResult))
		return;
	myResult = DX11_DEVICE->CreateRenderTargetView(myTexture.Get(), &targetDesc, &myRenderTarget);
	if (FAILED(myResult))
		return;

	D3D11_TEXTURE2D_DESC textureDesc{};
	myTexture->GetDesc(&textureDesc);
	myWidth = textureDesc.Width;
	myHeight = textureDesc.Height;
}

void BackBuffer::Present() const
{
	if (mySwapChain)
		mySwapChain->Present(0, 0);
}

void BackBuffer::Clear()
{
	static constexpr FLOAT color[]{ 0.f, 0.f, 0.f, 0.f };
	DX11_CONTEXT->ClearRenderTargetView(myRenderTarget.Get(), color);
}

BackBuffer::operator std::span<const RenderTargetPtr>() const
{
	return { std::addressof(myRenderTarget), 1 };
}

BackBuffer::operator bool() const
{
	{ return SUCCEEDED(myResult); }
}

