#include "pch.h"
#include "BackBuffer.h"
#include "DX11.h"

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
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = 0;

	ComPtr<IDXGIFactory> factory{};
	myResult = CreateDXGIFactory(IID_PPV_ARGS(&factory));
	if (FAILED(myResult))
		return;

	myResult = factory->CreateSwapChain(DX11::GetDevice(), &swapChainDesc, &mySwapChain);
	if (FAILED(myResult))
		return;

	Resize();
}

void BackBuffer::Resize()
{
	if (!mySwapChain)
		return;

	myTexture = nullptr;
	myRenderTarget = nullptr;
	myWidth = 0;
	myHeight = 0;

	myResult = mySwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(myResult))
		return;

	myResult = mySwapChain->GetBuffer(0, IID_PPV_ARGS(&myTexture));
	if (FAILED(myResult))
		return;

	D3D11_TEXTURE2D_DESC textureDesc{};
	myTexture->GetDesc(&textureDesc);
	myWidth = textureDesc.Width;
	myHeight = textureDesc.Height;

	D3D11_RENDER_TARGET_VIEW_DESC targetDesc{};
	targetDesc.Format = textureDesc.Format;
	targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	targetDesc.Texture2D.MipSlice = 0;

	myResult = DX11::GetDevice()->CreateRenderTargetView(myTexture.Get(), &targetDesc, &myRenderTarget);
	if (FAILED(myResult))
		return;
}

void BackBuffer::Present() const
{
	if (mySwapChain)
		mySwapChain->Present(0, 0);
}

void BackBuffer::Clear()
{
	static constexpr FLOAT color[]{ 0.f, 0.f, 0.f, 0.f };
	DX11::GetContext()->ClearRenderTargetView(myRenderTarget.Get(), color);
}

Viewport BackBuffer::GetViewport() const
{
	return Viewport{ CD3D11_VIEWPORT{ myTexture.Get(), myRenderTarget.Get() } };
}

BackBuffer::operator bool() const
{
	{ return SUCCEEDED(myResult); }
}

