#include "pch.h"
#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer(size_t aByteWidth)
	: myBuffer{}
	, myByteWidth{ aByteWidth }
{
	assert(aByteWidth != 0 && "ByteWidth cannot be 0");
	assert(aByteWidth % 16 == 0 && "ByteWidth must be a multiple of 16");

	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = static_cast<UINT>(aByteWidth);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	DX11_DEVICE->CreateBuffer(&desc, NULL, &myBuffer);
}

void ConstantBuffer::VSSetConstantBuffer(UINT aSlot) const
{
	DX11_CONTEXT->VSSetConstantBuffers(aSlot, 1, myBuffer.GetAddressOf());
}

void ConstantBuffer::PSSetConstantBuffer(UINT aSlot) const
{
	DX11_CONTEXT->PSSetConstantBuffers(aSlot, 1, myBuffer.GetAddressOf());
}
