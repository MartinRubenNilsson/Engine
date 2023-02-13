#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Mesh
{
public:
	using Ptr = std::shared_ptr<const Mesh>;

	Mesh(const aiMesh&);

	void Draw(const Matrix& aTransform) const;

	std::string_view GetName() const { return myName; }

	explicit operator bool() const;

private:
	std::string myName{};
	VertexBuffer myVertexBuffer{};
	IndexBuffer myIndexBuffer{};
};

