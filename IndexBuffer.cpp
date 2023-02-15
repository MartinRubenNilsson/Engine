#include "pch.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(std::span<unsigned> someIndices)
{
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = static_cast<UINT>(someIndices.size_bytes());
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = someIndices.data();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	DX11_DEVICE->CreateBuffer(&desc, &data, &myBuffer);
	if (!myBuffer)
		return;

	myIndexCount = static_cast<unsigned>(someIndices.size());
}

void IndexBuffer::SetIndexBuffer() const
{
	DX11_CONTEXT->IASetIndexBuffer(myBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}
