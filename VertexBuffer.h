#pragma once

class VertexBuffer
{
public:
	template<class VertexType, size_t Extent>
	VertexBuffer(std::span<VertexType, Extent> someVertices);

	void SetVertexBuffer() const;

	std::type_index GetVertexType() const { return myVertexType; }
	size_t GetVertexSize() const { return myVertexSize; }
	size_t GetVertexCount() const { return myVertexCount; }

	operator bool() const { return myBuffer; }

private:
	VertexBuffer(std::type_index aVertexType, size_t aVertexSize, size_t aVertexCount, const void* someData);

	std::type_index myVertexType;
	size_t myVertexSize;
	size_t myVertexCount;
	ComPtr<ID3D11Buffer> myBuffer;
};

template<class VertexType, size_t Extent>
inline VertexBuffer::VertexBuffer(std::span<VertexType, Extent> someVertices)
	: VertexBuffer{ typeid(VertexType), sizeof(VertexType), someVertices.size(), someVertices.data() }
{
}
