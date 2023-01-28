#include "pch.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(std::span<const UINT> someIndices)
	: myBuffer{}
	, myIndexCount{ someIndices.size() }
{
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = static_cast<UINT>(someIndices.size_bytes());
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = someIndices.data();

	DX11_DEVICE->CreateBuffer(&desc, &initialData, &myBuffer);
}

void IndexBuffer::SetIndexBuffer() const
{
	DX11_CONTEXT->IASetIndexBuffer(myBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}
