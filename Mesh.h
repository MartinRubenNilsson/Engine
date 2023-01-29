#pragma once

class Mesh
{
public:
	Mesh(const aiMesh&);

	void Draw() const;
	size_t GetVertexCount() const;
	size_t GetIndexCount() const;

	operator bool() const;

private:
	std::unique_ptr<class VertexBuffer> myVertexBuffer;
	std::unique_ptr<class IndexBuffer> myIndexBuffer;
};

