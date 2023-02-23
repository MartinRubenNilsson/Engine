#pragma once

class Cubemap
{
public:
	Cubemap() = default;
	Cubemap(std::span<const fs::path, 6> someImagePaths);

	void DrawSkybox() const;

	ShaderResourcePtr GetResource() const { return myShaderResource; }

	explicit operator bool() const;

private:
	HRESULT myResult{ E_FAIL };
	TexturePtr myTexture{};
	ShaderResourcePtr myShaderResource{};
	RenderTargetPtr myRenderTarget{};
	unsigned myWidth{}, myHeight{};
};

