#pragma once

class DepthBuffer
{
public:
	DepthBuffer() = default;
	DepthBuffer(unsigned aWidth, unsigned aHeight);

	void Clear(float aDepth = 0.f); // 0.f since we use a reversed Z buffer

	operator DepthStencilPtr() const { return myDepthStencil; }

	explicit operator bool() const;

private:
	HRESULT myResult{ E_FAIL };
	TexturePtr myTexture{};
	DepthStencilPtr myDepthStencil{};
	unsigned myWidth{}, myHeight{};
};

