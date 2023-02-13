#pragma once

class VertexBuffer
{
public:
	VertexBuffer() = default;

	template<class VertexType, size_t Extent>
	VertexBuffer(std::span<VertexType, Extent> someVertices);

	void SetVertexBuffer() const;

	unsigned GetVertexSize() const { return myVertexSize; }
	unsigned GetVertexCount() const { return myVertexCount; }

	explicit operator bool() const { return myBuffer; }

private:
	VertexBuffer(unsigned aVertexSize, unsigned aVertexCount, const void* someData);

	BufferPtr myBuffer{};
	unsigned myVertexSize{}, myVertexCount{};
};

template<class VertexType, size_t Extent>
inline VertexBuffer::VertexBuffer(std::span<VertexType, Extent> someVertices)
	: VertexBuffer{ sizeof(VertexType), static_cast<unsigned>(someVertices.size()), someVertices.data() }
{
}
