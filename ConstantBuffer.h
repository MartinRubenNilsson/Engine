#pragma once

class ConstantBuffer
{
public:
	ConstantBuffer(size_t aByteWidth);

	void UpdateConstantBuffer(const void* someData);
	void VSSetConstantBuffer(UINT aSlot) const;
	void PSSetConstantBuffer(UINT aSlot) const;
	size_t GetByteWidth() const { return myByteWidth; }

	operator bool() const { return myBuffer; }

private:
	ComPtr<ID3D11Buffer> myBuffer;
	size_t myByteWidth;
};

