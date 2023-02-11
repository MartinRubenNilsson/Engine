#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Mesh
{
public:
	Mesh(const aiMesh&);

	void Draw(const Matrix& aTransform) const;

	std::string_view GetName() const { return myName; }
	unsigned GetMaterialIndex() const { return myMaterialIndex; }

	explicit operator bool() const { return myVertexBuffer && myIndexBuffer; }

private:
	std::string myName;
	unsigned myMaterialIndex;
	std::unique_ptr<VertexBuffer> myVertexBuffer;
	std::unique_ptr<IndexBuffer> myIndexBuffer;
};

