#pragma once

class SwapChain
{
public:
	SwapChain(HWND);

	void Present();
	void SetRenderTarget(ID3D11DepthStencilView* aDepthStencil = nullptr);
	void ClearRenderTarget(const float aColor[4]);

	unsigned GetWidth() const;
	unsigned GetHeight() const;

	operator bool() const { return SUCCEEDED(myResult); }

private:
	HRESULT myResult;
	ComPtr<IDXGISwapChain> mySwapChain;
	ComPtr<ID3D11Texture2D> myTexture2d;
	ComPtr<ID3D11RenderTargetView> myRenderTargetView;
};

