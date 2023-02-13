#pragma once

class GeometryBuffer
{
public:
	GeometryBuffer() = default;
	GeometryBuffer(unsigned aWidth, unsigned aHeight);

	void Clear();

	operator std::span<const RenderTargetPtr>() const { return myRenderTargets; }
	operator std::span<const ShaderResourcePtr>() const { return myShaderResources; }

	explicit operator bool() const;

private:
	static constexpr std::array ourFormats
	{
		DXGI_FORMAT_R32G32B32A32_FLOAT, // World position
		DXGI_FORMAT_R32G32B32A32_FLOAT,  // Vertex normal
		DXGI_FORMAT_R32G32B32A32_FLOAT,  // Pixel normal
		DXGI_FORMAT_R8G8B8A8_UNORM,		// Albedo
		DXGI_FORMAT_R8G8B8A8_UNORM,		// Metallic + Roughness + AO + [Unused]
	};

	static constexpr size_t ourCount{ ourFormats.size() };

	static_assert(ourCount <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

	HRESULT myResult{ E_FAIL };
	std::array<TexturePtr, ourCount> myTextures{};
	std::array<RenderTargetPtr, ourCount> myRenderTargets{};
	std::array<ShaderResourcePtr, ourCount> myShaderResources{};
	unsigned myWidth{}, myHeight{};
};

