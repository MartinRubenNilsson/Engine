#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(const aiMesh& aMesh)
	: myName{ aMesh.mName.C_Str() }
	, myMaterialIndex{ aMesh.mMaterialIndex }
	, myVertexBuffer{}
	, myIndexBuffer{}
{
	if (!aMesh.HasPositions())
		return;
	if (!aMesh.HasNormals())
		return;
	if (!aMesh.HasFaces())
		return;
	if (aMesh.mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
		return;
	if (aMesh.GetNumUVChannels() != 1)
		return;
	if (aMesh.mNumUVComponents[0] != 2)
		return;

	std::vector<BasicVertex> vertices(aMesh.mNumVertices);
	std::vector<unsigned> indices(3 * aMesh.mNumFaces);

	for (unsigned i = 0; i < aMesh.mNumVertices; ++i)
		std::memcpy(&vertices[i].position, &aMesh.mVertices[i], 3 * sizeof(float));

	for (unsigned i = 0; i < aMesh.mNumVertices; ++i)
		std::memcpy(&vertices[i].normal, &aMesh.mNormals[i], 3 * sizeof(float));

	for (unsigned i = 0; i < aMesh.mNumVertices; ++i)
		std::memcpy(&vertices[i].uv, &aMesh.mTextureCoords[0][i], 2 * sizeof(float));

	for (unsigned i = 0; i < aMesh.mNumFaces; ++i)
		std::memcpy(&indices[3 * i], aMesh.mFaces[i].mIndices, 3 * sizeof(unsigned));

	std::span vertexSpan{ vertices };
	std::span indexSpan{ indices };

	myVertexBuffer = std::make_unique<VertexBuffer>(vertexSpan);
	myIndexBuffer = std::make_unique<IndexBuffer>(indexSpan);
}

void Mesh::Draw(const Matrix& aTransform) const
{
	if (!operator bool())
		return;

	myVertexBuffer->SetVertexBuffer();
	myIndexBuffer->SetIndexBuffer();

	MeshBuffer buffer{};
	buffer.meshMatrix = aTransform;
	buffer.meshMatrixInverseTranspose = aTransform.Transpose().Invert();

	DX11_WRITE_CONSTANT_BUFFER(buffer);

	DX11_CONTEXT->DrawIndexed((UINT)myIndexBuffer->GetIndexCount(), 0, 0);
}

