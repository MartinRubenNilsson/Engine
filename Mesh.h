#pragma once

class Mesh
{
public:
	using Ptr = std::shared_ptr<Mesh>;

	static Ptr Create(const aiMesh&);

	void Draw(const Matrix& aTransform) const;

	const std::string& GetName() const { return myName; }
	size_t GetVertexCount() const;
	size_t GetIndexCount() const;

	operator bool() const;

private:
	Mesh(const aiMesh&);

	const std::string myName;
	std::shared_ptr<class VertexBuffer> myVertexBuffer;
	std::shared_ptr<class IndexBuffer> myIndexBuffer;
};

