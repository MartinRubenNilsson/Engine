#pragma once

class Mesh
{
public:
	Mesh(const aiMesh&);

	/*
	* Sets vertex and index buffers, then dispatches a draw call.
	* Make sure to update constant buffers and set shaders before calling.
	*/
	void Draw() const;

	std::string_view GetName() const { return myName; }
	unsigned GetVertexCount() const { return myVertexCount; }
	unsigned GetIndexCount() const { return myIndexCount; }
	unsigned GetTriangleCount() const { return myTriangleCount; }
	const BoundingBox& GetBoundingBox() const { return myBoundingBox; }

	explicit operator bool() const;

private:
	std::string myName{};
	HRESULT myResult{ E_FAIL };
	BufferPtr myVertexBuffer{}, myIndexBuffer{};
	unsigned myVertexCount{}, myIndexCount{}, myTriangleCount{};
	BoundingBox myBoundingBox{};
};

namespace ImGui
{
	void Inspect(const Mesh& aMesh);
}

