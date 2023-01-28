#pragma once

class VertexBuffer
{
public:
	template<class T, size_t N>
	VertexBuffer(std::span<T, N> someVertices);

	void SetVertexBuffer() const;
	size_t GetVertexSize() const { return myVertexSize; }
	size_t GetVertexCount() const { return myVertexCount; }

	operator bool() const { return myBuffer; }

private:
	VertexBuffer(const void* someData, size_t aVertexSize, size_t aVertexCount);

	ComPtr<ID3D11Buffer> myBuffer;
	size_t myVertexSize;
	size_t myVertexCount;
};

template<class T, size_t N>
inline VertexBuffer::VertexBuffer(std::span<T, N> someVertices)
	: VertexBuffer{ someVertices.data(), sizeof(T), someVertices.size() }
{
}
