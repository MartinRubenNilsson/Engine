#pragma once

class RenderTargets
{
public:
	RenderTargets() = default;
	RenderTargets(unsigned aWidth, unsigned aHeight, std::span<const DXGI_FORMAT> someFormats);

	void Clear();

	operator std::span<const RenderTargetPtr>() const { return myRenderTargets; }
	operator std::span<const ShaderResourcePtr>() const { return myShaderResources; }

	explicit operator bool() const;

private:
	static constexpr size_t ourCount{ D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT };

	HRESULT myResult{ E_FAIL };
	std::array<TexturePtr, ourCount> myTextures{};
	std::array<RenderTargetPtr, ourCount> myRenderTargets{};
	std::array<ShaderResourcePtr, ourCount> myShaderResources{};
	unsigned myWidth{}, myHeight{};
};

