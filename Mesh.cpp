#include "pch.h"
#include "Mesh.h"

/*
* class Mesh
*/

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

	BoundingBox::CreateFromPoints(
		myBoundingBox,
		{ aMesh.mAABB.mMin.x, aMesh.mAABB.mMin.y, aMesh.mAABB.mMin.z },
		{ aMesh.mAABB.mMax.x, aMesh.mAABB.mMax.y, aMesh.mAABB.mMax.z }
	);
}

void Mesh::SetBuffers() const
{
	myVertexBuffer.SetVertexBuffer();
	myIndexBuffer.SetIndexBuffer();
}

std::string_view Mesh::GetName() const
{
	return myName;
}

unsigned Mesh::GetVertexCount() const
{
	return myVertexBuffer.GetVertexCount();
}

unsigned Mesh::GetIndexCount() const
{
	return myIndexBuffer.GetIndexCount();
}

const BoundingBox& Mesh::GetBoundingBox() const
{
	return myBoundingBox;
}

Mesh::operator bool() const
{
	return myVertexBuffer && myIndexBuffer;
}

/*
* namespace ImGui
*/

void ImGui::InspectMesh(const Mesh& aMesh)
{
	auto& box = aMesh.GetBoundingBox();

	Text("Name: %s", aMesh.GetName().data());
	Value("Vertices", aMesh.GetVertexCount());
	Value("Indices", aMesh.GetIndexCount());
	Text("Center: (%.3g, %.3g, %.3g)", box.Center.x, box.Center.y, box.Center.z);
	Text("Extents: (%.3g, %.3g, %.3g)", box.Extents.x, box.Extents.y, box.Extents.z);
}
