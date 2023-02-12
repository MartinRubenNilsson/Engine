#include "pch.h"
#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer(size_t aByteWidth)
	: myByteWidth{ aByteWidth }
{
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = static_cast<UINT>(aByteWidth);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	DX11_DEVICE->CreateBuffer(&desc, NULL, &myBuffer);
}

void ConstantBuffer::WriteConstantBuffer(const void* someData)
{
	D3D11_MAPPED_SUBRESOURCE subresource{};
	DX11_CONTEXT->Map(myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	std::memcpy(subresource.pData, someData, myByteWidth);
	DX11_CONTEXT->Unmap(myBuffer.Get(), 0);
}

void ConstantBuffer::VSSetConstantBuffer(unsigned aSlot) const
{
	DX11_CONTEXT->VSSetConstantBuffers(aSlot, 1, myBuffer.GetAddressOf());
}

void ConstantBuffer::PSSetConstantBuffer(unsigned aSlot) const
{
	DX11_CONTEXT->PSSetConstantBuffers(aSlot, 1, myBuffer.GetAddressOf());
}
