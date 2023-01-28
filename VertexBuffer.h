#pragma once

class VertexBuffer
{
public:
	template<class T, size_t N>
	VertexBuffer(std::span<T, N> someVertices);

	void SetVertexBuffer() const;
	size_t GetVertexCount() const { return myVertexCount; }
	size_t GetVertexSize() const { return myVertexSize; }

	operator bool() const { return myBuffer; }

private:
	VertexBuffer(std::span<const std::byte> someData, size_t aVertexCount, size_t aVertexSize);

	ComPtr<ID3D11Buffer> myBuffer;
	size_t myVertexCount;
	size_t myVertexSize;
};

template<class T, size_t N>
inline VertexBuffer::VertexBuffer(std::span<T, N> someVertices)
	: VertexBuffer{ std::as_bytes(someVertices), someVertices.size(), sizeof(T) }
{
}
