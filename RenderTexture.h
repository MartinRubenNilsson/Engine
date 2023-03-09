#pragma once

class RenderTexture
{
public:
	RenderTexture() = default;
	RenderTexture(unsigned aWidth, unsigned aHeight, DXGI_FORMAT aFormat);

	void Clear(const Color& aColor = {});
	void GetTexel(std::span<std::byte> aBuffer, unsigned x, unsigned y);

	Viewport GetViewport() const;

	operator TexturePtr() const { return myTexture; }
	operator ShaderResourcePtr() const { return myShaderResource; }
	operator RenderTargetPtr() const { return myRenderTarget; }

	explicit operator bool() const;

private:
	bool CreateTexel();

	HRESULT myResult{ E_FAIL };
	TexturePtr myTexture{}, myTexel{};
	RenderTargetPtr myRenderTarget{};
	ShaderResourcePtr myShaderResource{};
	unsigned myWidth{}, myHeight{};
};

