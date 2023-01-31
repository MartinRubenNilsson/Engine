#pragma once

class ConstantBuffer
{
public:
	ConstantBuffer(size_t aByteWidth);

	void WriteConstantBuffer(const void* someData);
	void VSSetConstantBuffer(unsigned aSlot) const;
	void PSSetConstantBuffer(unsigned aSlot) const;
	size_t GetByteWidth() const { return myByteWidth; }

	operator bool() const { return myBuffer; }

private:
	ComPtr<ID3D11Buffer> myBuffer;
	size_t myByteWidth;
};

