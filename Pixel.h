#pragma once

class Pixel
{
public:
	Pixel() = default;
	Pixel(DXGI_FORMAT);

	void Copy(TexturePtr aTexture, unsigned x, unsigned y);
	void Read(void* someData, size_t aSize) const;

	explicit operator bool() const { return myTexture; }

private:
	TexturePtr myTexture{};
};

