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

	const unsigned vertexCount{ aMesh.mNumVertices };
	const unsigned indexCount{ aMesh.mNumFaces * 3 };
	const unsigned triangleCount{ aMesh.mNumFaces };

	// Create vertex buffer
	{
		std::vector<VsInBasic> vertices(vertexCount);

		for (unsigned i = 0; i < vertexCount; ++i)
		{
			std::memcpy(&vertices[i].position, &aMesh.mVertices[i], sizeof(Vector3));
			std::memcpy(&vertices[i].normal, &aMesh.mNormals[i], sizeof(Vector3));
			std::memcpy(&vertices[i].tangent, &aMesh.mTangents[i], sizeof(Vector3));
			std::memcpy(&vertices[i].bitangent, &aMesh.mBitangents[i], sizeof(Vector3));
			std::memcpy(&vertices[i].uv, &aMesh.mTextureCoords[0][i], sizeof(Vector2));
		}

		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = vertexCount * sizeof(VsInBasic);
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data{};
		data.pSysMem = vertices.data();

		myResult = DX11_DEVICE->CreateBuffer(&desc, &data, &myVertexBuffer);
		if (FAILED(myResult))
			return;
	}

	// Create index buffer
	{
		std::vector<unsigned> indices(indexCount);

		for (unsigned i = 0; i < triangleCount; ++i)
			std::memcpy(&indices[3 * i], aMesh.mFaces[i].mIndices, 3 * sizeof(unsigned));

		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = indexCount * sizeof(unsigned);
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data{};
		data.pSysMem = indices.data();

		myResult = DX11_DEVICE->CreateBuffer(&desc, &data, &myIndexBuffer);
		if (FAILED(myResult))
			return;
	}

	myVertexCount = vertexCount;
	myIndexCount = indexCount;
	myTriangleCount = triangleCount;

	BoundingBox::CreateFromPoints(
		myBoundingBox,
		{ aMesh.mAABB.mMin.x, aMesh.mAABB.mMin.y, aMesh.mAABB.mMin.z },
		{ aMesh.mAABB.mMax.x, aMesh.mAABB.mMax.y, aMesh.mAABB.mMax.z }
	);
}

void Mesh::Draw() const
{
	if (!operator bool())
		return;

	static constexpr UINT stride{ sizeof(VsInBasic) };
	static constexpr UINT offset{ 0 };

	DX11_CONTEXT->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	DX11_CONTEXT->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	DX11_CONTEXT->DrawIndexed(myIndexCount, 0, 0);
}

Mesh::operator bool() const
{
	return SUCCEEDED(myResult);
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
	Value("Triangles", aMesh.GetTriangleCount());
	Text("Center: (%.2g, %.2g, %.2g)", box.Center.x, box.Center.y, box.Center.z);
	Text("Extents: (%.2g, %.2g, %.2g)", box.Extents.x, box.Extents.y, box.Extents.z);
}
