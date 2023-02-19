#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Mesh
{
public:
	using Ptr = std::shared_ptr<const Mesh>;

	Mesh(const aiMesh&);

	void SetVertexAndIndexBuffers() const;

	std::string_view GetName() const;
	unsigned GetVertexCount() const;
	unsigned GetIndexCount() const;
	const BoundingBox& GetBoundingBox() const;

	explicit operator bool() const;

private:
	std::string myName{};
	VertexBuffer myVertexBuffer{};
	IndexBuffer myIndexBuffer{};
	BoundingBox myBoundingBox{};
};

namespace ImGui
{
	void InspectMesh(const Mesh& aMesh);
}
