#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(const aiMesh& aMesh)
	: myName{ aMesh.mName.C_Str() }
{
	assert(aMesh.HasPositions());
	assert(aMesh.HasNormals());
	assert(aMesh.HasTangentsAndBitangents());
	assert(aMesh.GetNumUVChannels() == 1);
	assert(aMesh.mNumUVComponents[0] == 2);
	assert(aMesh.HasFaces());
	assert(aMesh.mPrimitiveTypes == aiPrimitiveType_TRIANGLE);

	std::vector<BasicVertex> vertices(aMesh.mNumVertices);
	std::vector<unsigned> indices(3 * aMesh.mNumFaces);

	for (unsigned i = 0; i < aMesh.mNumVertices; ++i)
	{
		std::memcpy(&vertices[i].position,	&aMesh.mVertices[i],		 sizeof(Vector3));
		std::memcpy(&vertices[i].normal,	&aMesh.mNormals[i],			 sizeof(Vector3));
		std::memcpy(&vertices[i].tangent,	&aMesh.mTangents[i],		 sizeof(Vector3));
		std::memcpy(&vertices[i].bitangent, &aMesh.mBitangents[i],		 sizeof(Vector3));
		std::memcpy(&vertices[i].uv,		&aMesh.mTextureCoords[0][i], sizeof(Vector2));
	}

	for (unsigned i = 0; i < aMesh.mNumFaces; ++i)
		std::memcpy(&indices[3 * i], aMesh.mFaces[i].mIndices, 3 * sizeof(unsigned));

	myVertexBuffer = { std::span{ vertices } };
	myIndexBuffer = { std::span{ indices } };
}

void Mesh::Draw(const Matrix& aTransform) const
{
	if (!operator bool())
		return;

	myVertexBuffer.SetVertexBuffer();
	myIndexBuffer.SetIndexBuffer();

	MeshBuffer buffer{};
	buffer.meshMatrix = aTransform;
	buffer.meshMatrixInverseTranspose = aTransform.Transpose().Invert();

	DX11_WRITE_CONSTANT_BUFFER(buffer);

	DX11_CONTEXT->DrawIndexed(myIndexBuffer.GetIndexCount(), 0, 0);
}

Mesh::operator bool() const
{
	return myVertexBuffer && myIndexBuffer;
}

