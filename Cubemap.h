#pragma once

class Cubemap
{
public:
	Cubemap() = default;
	Cubemap(std::span<const fs::path, 6> someImagePaths);

	void DrawSkybox() const;

	operator std::span<const ShaderResourcePtr>() const;

	explicit operator bool() const;

private:
	HRESULT myResult{ E_FAIL };
	TexturePtr myTexture{};
	ShaderResourcePtr myShaderResource{};
	RenderTargetPtr myRenderTarget{};
	unsigned myWidth{}, myHeight{};
};

