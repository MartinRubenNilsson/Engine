#include "pch.h"
#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer(unsigned aByteWidth)
{
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = aByteWidth;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	DX11_DEVICE->CreateBuffer(&desc, NULL, &myBuffer);
	if (!myBuffer)
		return;

	myByteWidth = aByteWidth;
}

void ConstantBuffer::Update(const void* someData)
{
	D3D11_MAPPED_SUBRESOURCE subresource{};
	DX11_CONTEXT->Map(myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	std::memcpy(subresource.pData, someData, myByteWidth);
	DX11_CONTEXT->Unmap(myBuffer.Get(), 0);
}

void ConstantBuffer::VSSetBuffer(unsigned aSlot) const
{
	DX11_CONTEXT->VSSetConstantBuffers(aSlot, 1, myBuffer.GetAddressOf());
}

void ConstantBuffer::PSSetBuffer(unsigned aSlot) const
{
	DX11_CONTEXT->PSSetConstantBuffers(aSlot, 1, myBuffer.GetAddressOf());
}
