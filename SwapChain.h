#pragma once

class SwapChain
{
public:
	SwapChain(HWND);

	void ClearRenderTargets(const Color& aColor = { 0.f, 0.f, 0.f, 0.f });
	void Present() const;

	void GetDimensions(unsigned& aWidth, unsigned& aHeight) const;

	operator std::span<const RenderTargetPtr>() const;

	explicit operator bool() const { return SUCCEEDED(myResult); }

private:
	HRESULT myResult{ E_FAIL };
	ComPtr<IDXGISwapChain> mySwapChain{};
	TexturePtr myBackBuffer{};
	RenderTargetPtr myRenderTarget{};
	unsigned myWidth{}, myHeight{};
};

