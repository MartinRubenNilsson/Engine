#pragma once

class Mesh
{
public:
	Mesh(const aiMesh&);
	Mesh(const fs::path&);
	~Mesh();

	void Draw() const;
	size_t GetVertexCount() const;
	size_t GetIndexCount() const;
	const std::string& GetName() { return myName; }

	operator bool() const;

private:
	Mesh(Mesh&&);
	Mesh& operator=(Mesh&&);
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	std::unique_ptr<class VertexBuffer> myVertexBuffer;
	std::unique_ptr<class IndexBuffer> myIndexBuffer;
	std::string myName;
};

