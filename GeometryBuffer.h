#pragma once

class GeometryBuffer
{
public:
	GeometryBuffer(unsigned aWidth, unsigned aHeight);
	~GeometryBuffer();

	void ClearRenderTargets(const Color& aColor = { 0.f, 0.f, 0.f, 0.f });
	auto GetRenderTargets() const { return std::span(myRenderTargets); }
	auto GetShaderResources() const { return std::span(myShaderResources); }

	operator bool() const { return SUCCEEDED(myResult); }

private:
	static constexpr std::array ourFormats =
	{
		DXGI_FORMAT_R32G32B32A32_FLOAT, // World position
		DXGI_FORMAT_R32G32B32A32_FLOAT, // World normal
	};

	static_assert(ourFormats.size() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

	HRESULT myResult;
	ID3D11Texture2D* myTextures[ourFormats.size()];
	ID3D11RenderTargetView* myRenderTargets[ourFormats.size()];
	ID3D11ShaderResourceView* myShaderResources[ourFormats.size()];
	unsigned myWidth;
	unsigned myHeight;
};

