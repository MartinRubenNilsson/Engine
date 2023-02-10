#pragma once
#include "Image.h"

class Cubemap
{
public:
	Cubemap(std::span<const Image, 6> someFaces);

	void DrawSkybox() const;

	operator std::span<const ShaderResourcePtr>() const;

	operator bool() const { return SUCCEEDED(myResult); }

private:
	HRESULT myResult;
	TexturePtr myTexture;
	ShaderResourcePtr myShaderResource;
	unsigned myWidth, myHeight;
};

