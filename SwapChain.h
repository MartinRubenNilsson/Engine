#pragma once

class SwapChain
{
public:
	SwapChain(HWND);

	void ClearRenderTargets(const Color& aColor = { 0.f, 0.f, 0.f, 0.f });
	void Present() const;

	void GetDimensions(unsigned& aWidth, unsigned& aHeight) const;

	operator std::span<const RenderTargetPtr>() const;

	operator bool() const { return SUCCEEDED(myResult); }

private:
	HRESULT myResult;
	SwapChainPtr mySwapChain;
	TexturePtr myBackBuffer;
	RenderTargetPtr myRenderTarget;
	unsigned myWidth, myHeight;
};

