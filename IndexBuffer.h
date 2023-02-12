#pragma once

class IndexBuffer
{
public:
	IndexBuffer(std::span<unsigned> someIndices);

	void SetIndexBuffer(unsigned anOffset = 0) const;
	size_t GetIndexCount() const { return myIndexCount; }

	explicit operator bool() const { return myBuffer; }

private:
	size_t myIndexCount{};
	BufferPtr myBuffer{};
};
