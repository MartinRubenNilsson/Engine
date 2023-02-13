#pragma once

class IndexBuffer
{
public:
	IndexBuffer() = default;
	IndexBuffer(std::span<unsigned> someIndices);

	void SetIndexBuffer(unsigned anOffset = 0) const;

	unsigned GetIndexCount() const { return myIndexCount; }

	explicit operator bool() const { return myBuffer; }

private:
	BufferPtr myBuffer{};
	unsigned myIndexCount{};
};
