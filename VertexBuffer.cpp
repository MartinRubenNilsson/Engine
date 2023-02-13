#include "pch.h"
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(unsigned aVertexSize, unsigned aVertexCount, const void* someData)
{
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = aVertexSize * aVertexCount;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = someData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	DX11_DEVICE->CreateBuffer(&desc, &data, &myBuffer);
	if (!myBuffer)
		return;

	myVertexSize = aVertexSize;
	myVertexCount = aVertexCount;
}

void VertexBuffer::SetVertexBuffer() const
{
	static constexpr UINT offset{ 0 };
	DX11_CONTEXT->IASetVertexBuffers(0, 1, myBuffer.GetAddressOf(), &myVertexSize, &offset);
}
