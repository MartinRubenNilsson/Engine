#pragma once

class SwapChain
{
public:
	SwapChain(HWND);

	void ClearRenderTargets(const Color& aColor = { 0.f, 0.f, 0.f, 0.f });
	void Present() const;

	void GetDimensions(unsigned& aWidth, unsigned& aHeight) const;

	operator std::span<ID3D11RenderTargetView* const>() const;

	operator bool() const { return SUCCEEDED(myResult); }

private:
	HRESULT myResult;
	ComPtr<IDXGISwapChain> mySwapChain;
	ComPtr<ID3D11Texture2D> myBackBuffer;
	ComPtr<ID3D11RenderTargetView> myRenderTarget;
	unsigned myWidth, myHeight;
};

