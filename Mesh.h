#pragma once

class Mesh
{
public:
	using Ptr = std::shared_ptr<const Mesh>;

	Mesh(const aiMesh&);

	void Draw() const;

	std::string_view GetName() const { return myName; }
	unsigned GetVertexCount() const { return myVertexCount; }
	unsigned GetIndexCount() const { return myIndexCount; }
	const BoundingBox& GetBoundingBox() const { return myBoundingBox; }

	explicit operator bool() const;

private:
	std::string myName{};
	HRESULT myResult{ E_FAIL };
	BufferPtr myVertexBuffer{}, myIndexBuffer{};
	unsigned myVertexCount{}, myIndexCount{};
	BoundingBox myBoundingBox{};
};

namespace ImGui
{
	void InspectMesh(const Mesh& aMesh);
}
