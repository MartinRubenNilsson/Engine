#pragma once

class DepthBuffer
{
public:
	DepthBuffer() = default;
	DepthBuffer(unsigned aWidth, unsigned aHeight);

	void Clear(float aDepth);

	operator DepthStencilPtr() const { return myDepthStencil; }

	explicit operator bool() const;

private:
	HRESULT myResult{ E_FAIL };
	TexturePtr myTexture{};
	DepthStencilPtr myDepthStencil{};
	unsigned myWidth{}, myHeight{};
};

