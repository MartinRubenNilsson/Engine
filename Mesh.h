#pragma once

class Mesh
{
public:
	Mesh(const aiMesh&);
	Mesh(const fs::path&);
	Mesh(Mesh&&);
	Mesh& operator=(Mesh&&);
	~Mesh();

	void Draw() const;
	size_t GetVertexCount() const;
	size_t GetIndexCount() const;

	operator bool() const;

private:
	std::unique_ptr<class VertexBuffer> myVertexBuffer;
	std::unique_ptr<class IndexBuffer> myIndexBuffer;
};

