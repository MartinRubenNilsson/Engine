#pragma once

class VertexBuffer;
class IndexBuffer;

class Mesh
{
public:
	Mesh(const aiMesh&);

	void Draw(const Matrix& aTransform) const;

	const std::string& GetName() const { return myName; }
	unsigned GetMaterialIndex() const { return myMaterialIndex; }
	size_t GetVertexCount() const;
	size_t GetIndexCount() const;

	operator bool() const;

private:
	std::string myName;
	unsigned myMaterialIndex;
	std::shared_ptr<VertexBuffer> myVertexBuffer;
	std::shared_ptr<IndexBuffer> myIndexBuffer;
};

