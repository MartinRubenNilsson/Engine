#pragma once

class GeometryBuffer
{
public:
	GeometryBuffer(unsigned aWidth, unsigned aHeight);

	void ClearRenderTargets(const Color& aColor = { 0.f, 0.f, 0.f, 0.f });

	operator std::span<const RenderTargetPtr>() const { return myRenderTargets; }
	operator std::span<const ShaderResourcePtr>() const { return myShaderResources; }

	operator bool() const { return SUCCEEDED(myResult); }

private:
	static constexpr std::array ourFormats =
	{
		DXGI_FORMAT_R32G32B32A32_FLOAT, // World position
		DXGI_FORMAT_R32G32B32A32_FLOAT, // World normal
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, // Diffuse
	};

	static_assert(ourFormats.size() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

	HRESULT myResult;
	TexturePtr myTextures[ourFormats.size()];
	RenderTargetPtr myRenderTargets[ourFormats.size()];
	ShaderResourcePtr myShaderResources[ourFormats.size()];
	unsigned myWidth, myHeight;
};

