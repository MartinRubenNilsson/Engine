#include "pch.h"
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(const void* someData, size_t aVertexSize, size_t aVertexCount)
	: myBuffer{}
	, myVertexSize{ aVertexSize }
	, myVertexCount{ aVertexCount }
{
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = static_cast<UINT>(aVertexSize * aVertexCount);
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = someData;

	DX11_DEVICE->CreateBuffer(&desc, &data, &myBuffer);
}

void VertexBuffer::SetVertexBuffer() const
{
	const UINT stride{ static_cast<UINT>(myVertexSize) };
	const UINT offset{ 0 };
	DX11_CONTEXT->IASetVertexBuffers(0, 1, myBuffer.GetAddressOf(), &stride, &offset);
}
