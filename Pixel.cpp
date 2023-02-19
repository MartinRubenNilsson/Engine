#include "pch.h"
#include "Pixel.h"

Pixel::Pixel(DXGI_FORMAT aFormat)
{
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = 1;
	desc.Height = 1;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = aFormat;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;

	DX11_DEVICE->CreateTexture2D(&desc, NULL, &myTexture);
}

void Pixel::Copy(TexturePtr aTexture, unsigned x, unsigned y)
{
	if (!myTexture || !aTexture)
		return;

	D3D11_TEXTURE2D_DESC desc{};
	aTexture->GetDesc(&desc);

	if (x >= desc.Width || y >= desc.Height)
		return;

	D3D11_BOX box{};
	box.left = x;
	box.right = x + 1;
	box.top = y;
	box.bottom = y + 1;
	box.front = 0;
	box.back = 1;

	DX11_CONTEXT->CopySubresourceRegion(myTexture.Get(), 0, 0, 0, 0, aTexture.Get(), 0, &box);
}

void Pixel::Read(void* someData, size_t aSize) const
{
	D3D11_MAPPED_SUBRESOURCE resource{};
	DX11_CONTEXT->Map(myTexture.Get(), 0, D3D11_MAP_READ, 0, &resource);
	std::memcpy(someData, resource.pData, aSize);
	DX11_CONTEXT->Unmap(myTexture.Get(), 0);
}
