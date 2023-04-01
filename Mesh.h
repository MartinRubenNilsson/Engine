#pragma once

enum class MeshPrimitiveType
{
	Plane,
	Cube,
	Sphere,
	Cylinder,
	Cone,
	Torus,
	Suzanne,
	Count
};

const char* ToString(MeshPrimitiveType);

class Mesh
{
public:
	Mesh() = default;
	Mesh(MeshPrimitiveType);
	Mesh(const fs::path& aPath, std::string_view aName);
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

	fs::path myPath{}; // Path to the .fbx/.obj containing this mesh
	std::string myName{}; // Name of the node/transform on which this mesh sits
	BufferPtr myVertexBuffer{}, myIndexBuffer{};
	unsigned myVertexCount{}, myIndexCount{};
	BoundingBox myBoundingBox{};
	HRESULT myResult{ E_FAIL };
};

void to_json(json&, const Mesh&);
void from_json(const json&, Mesh&);

namespace ImGui
{
	void Inspect(Mesh& aMesh);
}

