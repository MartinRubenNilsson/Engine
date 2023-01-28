#include "pch.h"
#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer(UINT aByteWidth)
{
	assert(aByteWidth != 0 && "ByteWidth cannot be 0");
	assert(aByteWidth % 16 == 0 && "ByteWidth must be a multiple of 16");

	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = aByteWidth;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	DX11_DEVICE->CreateBuffer(&desc, NULL, &myBuffer);
}
