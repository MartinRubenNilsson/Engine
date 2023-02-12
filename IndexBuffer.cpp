#include "pch.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(std::span<unsigned> someIndices)
	: myIndexCount{ someIndices.size() }
{
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = static_cast<UINT>(someIndices.size_bytes());
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = someIndices.data();

	DX11_DEVICE->CreateBuffer(&desc, &data, &myBuffer);
}

void IndexBuffer::SetIndexBuffer(unsigned anOffset) const
{
	DX11_CONTEXT->IASetIndexBuffer(myBuffer.Get(), DXGI_FORMAT_R32_UINT, anOffset);
}
