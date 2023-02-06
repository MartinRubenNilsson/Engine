#pragma once
#include "Image.h"

class Cubemap
{
public:
	Cubemap(std::span<const Image, 6> someFaces);

	auto GetShaderResources() const { return std::span(myShaderResource.GetAddressOf(), 1); }

	operator bool() const { return SUCCEEDED(myResult); }

private:
	HRESULT myResult;
	ComPtr<ID3D11Texture2D> myTexture;
	ComPtr<ID3D11ShaderResourceView> myShaderResource;
	unsigned myWidth, myHeight;
};

