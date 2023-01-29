#pragma once

class IndexBuffer
{
public:
	IndexBuffer(std::span<const unsigned> someIndices);

	void SetIndexBuffer() const;
	size_t GetIndexCount() const { return myIndexCount; }

	operator bool() const { return myBuffer; }

private:
	ComPtr<ID3D11Buffer> myBuffer;
	size_t myIndexCount;
};
