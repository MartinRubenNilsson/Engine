#pragma once

class IndexBuffer
{
public:
	IndexBuffer() = default;
	IndexBuffer(std::span<unsigned> someIndices);

	void SetIndexBuffer() const;

	unsigned GetIndexCount() const { return myIndexCount; }

	explicit operator bool() const { return myBuffer; }

private:
	BufferPtr myBuffer{};
	unsigned myIndexCount{};
};
