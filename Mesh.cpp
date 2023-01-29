#include "pch.h"
#include "Mesh.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#pragma comment(lib, "assimp-vc142-mt") 

Mesh::Mesh(const aiMesh& aMesh)
	: myName{ aMesh.mName.C_Str() }
	, myMaterialIndex{ aMesh.mMaterialIndex }
	, myVertexBuffer{}
	, myIndexBuffer{}
{
	// Load vertices
	{
		struct Vertex
		{
			Vector3 position{};
			Vector3 normal{};
		};

		std::vector<Vertex> vertices(aMesh.mNumVertices);
		
		if (aMesh.HasPositions())
		{
			for (unsigned i = 0; i < aMesh.mNumVertices; ++i)
				std::memcpy(&vertices[i].position, &aMesh.mVertices[i], 3 * sizeof(float));
		}

		if (aMesh.HasNormals())
		{
			for (unsigned i = 0; i < aMesh.mNumVertices; ++i)
				std::memcpy(&vertices[i].normal, &aMesh.mNormals[i], 3 * sizeof(float));
		}

		myVertexBuffer = std::make_shared<VertexBuffer>(std::span(vertices));
	}

	// Load indices
	if (aMesh.HasFaces() && aMesh.mPrimitiveTypes == aiPrimitiveType_TRIANGLE)
	{
		std::vector<unsigned> indices(3 * aMesh.mNumFaces);

		for (unsigned i = 0; i < aMesh.mNumFaces; ++i)
			std::memcpy(&indices[3 * i], aMesh.mFaces[i].mIndices, 3 * sizeof(unsigned));

		myIndexBuffer = std::make_shared<IndexBuffer>(std::span(indices));
	}
}

void Mesh::Draw() const
{
	if (operator bool())
	{
		myVertexBuffer->SetVertexBuffer();
		myIndexBuffer->SetIndexBuffer();
		DX11_CONTEXT->DrawIndexed(static_cast<UINT>(myIndexBuffer->GetIndexCount()), 0, 0);
	}
}

size_t Mesh::GetVertexCount() const
{
	return myVertexBuffer ? myVertexBuffer->GetVertexCount() : 0;
}

size_t Mesh::GetIndexCount() const
{
	return myIndexBuffer ? myIndexBuffer->GetIndexCount() : 0;
}

Mesh::operator bool() const
{
	return myVertexBuffer && *myVertexBuffer && myIndexBuffer && *myIndexBuffer;
}
