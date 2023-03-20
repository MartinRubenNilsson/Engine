#pragma once

class Mesh
{
public:
	Mesh() = default;
	Mesh(const aiMesh&);

	/*
	* Sets vertex and index buffers, then dispatches a draw call.
	* Caller needs to update constant buffers and set shaders themselves before calling.
	*/
	void Draw() const;

	const fs::path& GetPath() const { return myPath; }
	std::string_view GetName() const { return myName; }
	unsigned GetVertexCount() const { return myVertexCount; }
	unsigned GetIndexCount() const { return myIndexCount; }
	const BoundingBox& GetBoundingBox() const { return myBoundingBox; }

	explicit operator bool() const;

private:
	friend class Scene;
	friend void to_json(json&, const Mesh&);
	friend void from_json(const json&, Mesh&);

	fs::path myPath{}; // Path to the .fbx/.obj containing this mesh
	std::string myName{}; // Name of the node/transform on which this mesh sits
	BufferPtr myVertexBuffer{}, myIndexBuffer{};
	unsigned myVertexCount{}, myIndexCount{};
	BoundingBox myBoundingBox{};
	HRESULT myResult{ E_FAIL };
};

namespace ImGui
{
	void Inspect(Mesh& aMesh);
}

