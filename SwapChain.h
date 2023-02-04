#pragma once

class SwapChain
{
public:
	SwapChain(HWND);

	void Present();

	void SetRenderTarget(ID3D11DepthStencilView* aDepthStencil = nullptr);

	void ClearRenderTarget(const Color& aColor);

	unsigned GetWidth() const;
	unsigned GetHeight() const;

	operator bool() const { return SUCCEEDED(myResult); }

private:
	ComPtr<IDXGISwapChain> mySwapChain;
	ComPtr<ID3D11Texture2D> myBackBuffer;
	ComPtr<ID3D11RenderTargetView> myRenderTarget;
	HRESULT myResult;
};

