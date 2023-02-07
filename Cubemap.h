#pragma once
#include "Image.h"

class Cubemap
{
public:
	Cubemap(std::span<const Image, 6> someFaces);

	void DrawSkybox() const;

	operator std::span<ID3D11ShaderResourceView* const>() const;

	operator bool() const { return SUCCEEDED(myResult); }

private:
	HRESULT myResult;
	ComPtr<ID3D11Texture2D> myTexture;
	ComPtr<ID3D11ShaderResourceView> myShaderResource;
	unsigned myWidth, myHeight;
};

