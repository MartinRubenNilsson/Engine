#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Mesh
{
public:
	Mesh(const aiMesh&);

	void Draw(const Matrix& aTransform) const;

	std::string_view GetName() const;
	size_t GetVertexCount() const;
	size_t GetIndexCount() const;

	operator bool() const { return myImpl.operator bool(); };

private:
	struct Impl
	{
		std::string name;
		VertexBuffer vertexBuffer;
		IndexBuffer indexBuffer;
	};

	std::shared_ptr<Impl> myImpl;
};

