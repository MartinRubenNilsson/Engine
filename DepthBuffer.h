#pragma once

class DepthBuffer
{
public:
	DepthBuffer(unsigned aWidth, unsigned aHeight);

	void Clear();

	operator DepthStencilPtr() const { return myDepthStencil; }

	explicit operator bool() const { return SUCCEEDED(myResult); }

private:
	HRESULT myResult{ E_FAIL };
	TexturePtr myTexture{};
	DepthStencilPtr myDepthStencil{};
};

