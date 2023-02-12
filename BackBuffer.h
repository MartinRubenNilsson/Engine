#pragma once

class BackBuffer
{
public:
	BackBuffer(HWND);

	void Present() const;
	void Clear();

	unsigned GetWidth() const { return myWidth; }
	unsigned GetHeight() const { return myHeight; }

	operator std::span<const RenderTargetPtr>() const;

	explicit operator bool() const { return SUCCEEDED(myResult); }

private:
	HRESULT myResult{ E_FAIL };
	ComPtr<IDXGISwapChain> mySwapChain{};
	TexturePtr myTexture{};
	RenderTargetPtr myRenderTarget{};
	unsigned myWidth{}, myHeight{};
};

