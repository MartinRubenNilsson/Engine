#pragma once

class Mesh
{
public:
	Mesh(const aiMesh&);
	~Mesh();

	void Draw(const Matrix& aTransform) const;

	const std::string& GetName() const { return myName; }
	size_t GetVertexCount() const;
	size_t GetIndexCount() const;

	operator bool() const;

private:
	std::string myName;
	std::unique_ptr<class VertexBuffer> myVertexBuffer;
	std::unique_ptr<class IndexBuffer> myIndexBuffer;
};

