#pragma once
#include "Image.h"

class Cubemap
{
public:
	Cubemap(std::span<const Image, 6> someFaces);

	void DrawSkybox() const;

	operator std::span<const ShaderResourcePtr>() const;

	explicit operator bool() const { return SUCCEEDED(myResult); }

private:
	HRESULT myResult{ E_FAIL };
	TexturePtr myTexture{};
	ShaderResourcePtr myShaderResource{};
	unsigned myWidth{}, myHeight{};
};

