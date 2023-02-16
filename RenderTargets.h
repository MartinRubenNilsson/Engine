#pragma once

class RenderTargets
{
public:
	RenderTargets() = default;
	RenderTargets(unsigned aWidth, unsigned aHeight, std::span<const DXGI_FORMAT> someFormats);

	void Clear();

	TexturePtr GetTexture(size_t anIndex) const;
	RenderTargetPtr GetRenderTarget(size_t anIndex) const;

	operator std::span<const TexturePtr>() const { return myTextures; }
	operator std::span<const RenderTargetPtr>() const { return myRenderTargets; }
	operator std::span<const ShaderResourcePtr>() const { return myShaderResources; }

	explicit operator bool() const;

private:
	HRESULT myResult{ E_FAIL };
	std::vector<TexturePtr> myTextures{};
	std::vector<RenderTargetPtr> myRenderTargets{};
	std::vector<ShaderResourcePtr> myShaderResources{};
	unsigned myWidth{}, myHeight{};
};

