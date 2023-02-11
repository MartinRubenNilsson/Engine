#pragma once

class IndexBuffer
{
public:
	IndexBuffer(std::span<unsigned> someIndices);

	void SetIndexBuffer() const;
	size_t GetIndexCount() const { return myIndexCount; }

	explicit operator bool() const { return myBuffer; }

private:
	ComPtr<ID3D11Buffer> myBuffer;
	size_t myIndexCount;
};
