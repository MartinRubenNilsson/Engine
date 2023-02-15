#pragma once

class ConstantBuffer
{
public:
	ConstantBuffer() = default;
	ConstantBuffer(unsigned aByteWidth);

	void WriteToBuffer(const void* someData);

	void VSSetBuffer(unsigned aSlot) const;
	void PSSetBuffer(unsigned aSlot) const;

	unsigned GetByteWidth() const { return myByteWidth; }

	explicit operator bool() const { return myBuffer; }

private:
	BufferPtr myBuffer{};
	unsigned myByteWidth{};
};

