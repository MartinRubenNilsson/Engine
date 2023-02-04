#pragma once

class GeometryBuffer
{
public:
	GeometryBuffer(unsigned aWidth, unsigned aHeight);

	operator bool() const { return SUCCEEDED(myResult); }

private:
	static constexpr std::array ourFormats =
	{
		DXGI_FORMAT_R32G32B32A32_FLOAT, // Position
		DXGI_FORMAT_R32G32B32A32_FLOAT, // Normal
	};

	ComPtr<ID3D11Texture2D> myTextures[ourFormats.size()];
	ComPtr<ID3D11RenderTargetView> myRenderTargets[ourFormats.size()];
	ComPtr<ID3D11ShaderResourceView> myShaderResources[ourFormats.size()];
	HRESULT myResult;
};

