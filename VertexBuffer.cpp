#include "pch.h"
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(std::span<const std::byte> someData, size_t aVertexCount, size_t aVertexSize)
	: myBuffer{}
	, myVertexCount{ aVertexCount }
	, myVertexSize{ aVertexSize }
{
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = (UINT)someData.size_bytes();
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = someData.data();

	DX11_DEVICE->CreateBuffer(&desc, &initialData, &myBuffer);
}

void VertexBuffer::SetVertexBuffer() const
{
	const UINT stride{ (UINT)myVertexSize };
	const UINT offset{ 0 };
	DX11_CONTEXT->IASetVertexBuffers(0, 1, myBuffer.GetAddressOf(), &stride, &offset);
}
