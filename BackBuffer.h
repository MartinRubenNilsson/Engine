#pragma once

class BackBuffer
{
public:
	BackBuffer() = default;
	BackBuffer(HWND);

	void Present() const;
	void Clear();

	Viewport GetViewport() const;
	RenderTargetPtr GetTarget() const { return myRenderTarget; }

	unsigned GetWidth() const { return myWidth; }
	unsigned GetHeight() const { return myHeight; }

	explicit operator bool() const;

private:
	HRESULT myResult{ E_FAIL };
	SwapChainPtr mySwapChain{};
	TexturePtr myTexture{};
	RenderTargetPtr myRenderTarget{};
	unsigned myWidth{}, myHeight{};
};

