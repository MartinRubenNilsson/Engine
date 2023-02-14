#pragma once

class BackBuffer
{
public:
	BackBuffer() = default;
	BackBuffer(HWND);

	void Present() const;
	void Clear();

	Viewport GetViewport() const;

	unsigned GetWidth() const { return myWidth; }
	unsigned GetHeight() const { return myHeight; }

	operator std::span<const RenderTargetPtr>() const;

	explicit operator bool() const;

private:
	HRESULT myResult{ E_FAIL };
	SwapChainPtr mySwapChain{};
	TexturePtr myTexture{};
	RenderTargetPtr myRenderTarget{};
	unsigned myWidth{}, myHeight{};
};

