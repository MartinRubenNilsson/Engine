#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(const aiMesh& aMesh)
{
	if (!aMesh.HasPositions())
		return;
	if (!aMesh.HasNormals())
		return;
	if (!aMesh.HasFaces())
		return;
	if (aMesh.mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
		return;

	std::vector<BasicVertex> vertices(aMesh.mNumVertices);
	std::vector<unsigned> indices(3 * aMesh.mNumFaces);

	for (unsigned i = 0; i < aMesh.mNumVertices; ++i)
		std::memcpy(&vertices[i].position, &aMesh.mVertices[i], 3 * sizeof(float));

	for (unsigned i = 0; i < aMesh.mNumVertices; ++i)
		std::memcpy(&vertices[i].normal, &aMesh.mNormals[i], 3 * sizeof(float));

	for (unsigned i = 0; i < aMesh.mNumFaces; ++i)
		std::memcpy(&indices[3 * i], aMesh.mFaces[i].mIndices, 3 * sizeof(unsigned));

	std::span vertexSpan{ vertices };
	std::span indexSpan{ indices };

	myImpl = std::make_shared<Impl>(aMesh.mName.C_Str(), vertexSpan, indexSpan);
}

void Mesh::Draw(const Matrix& aTransform) const
{
	if (!myImpl)
		return;

	myImpl->vertexBuffer.SetVertexBuffer();
	myImpl->indexBuffer.SetIndexBuffer();

	MeshBuffer buffer{};
	buffer.meshMatrix = aTransform;
	buffer.meshMatrixInverseTranspose = aTransform.Transpose().Invert();

	DX11_WRITE_CONSTANT_BUFFER(buffer);

	const UINT indexCount{ static_cast<UINT>(myImpl->indexBuffer.GetIndexCount()) };
	DX11_CONTEXT->DrawIndexed(indexCount, 0, 0);
}

std::string_view Mesh::GetName() const
{
	return myImpl ? myImpl->name : "";
}

size_t Mesh::GetVertexCount() const
{
	return myImpl ? myImpl->vertexBuffer.GetVertexCount() : 0;
}

size_t Mesh::GetIndexCount() const
{
	return myImpl ? myImpl->indexBuffer.GetIndexCount() : 0;
}
