#include "pch.h"
#include "Mesh.h"
#include "Scene.h"

/*
* class Mesh
*/

Mesh::Mesh(const fs::path& aPath, const aiMesh& aMesh)
	: myPath{ aPath }
	, myName{ aMesh.mName.C_Str() }
{
	if (!aMesh.HasPositions())
		return;
	if (!aMesh.HasFaces())
		return;
	if (aMesh.mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
		return;
	if (!aMesh.HasTangentsAndBitangents())
		return;
	if (aMesh.GetNumUVChannels() != 1)
		return;
	if (aMesh.mNumUVComponents[0] != 2)
		return;

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

void to_json(json& j, const Mesh& m)
{
	j["path"] = m.myPath;
	j["name"] = m.myName;
}

void from_json(const json& j, Mesh& m)
{
	const fs::path path = j.at("path");
	const std::string name = j.at("name");

	if (auto scene = SceneFactory::Get().GetAsset(path))
	{
		for (auto [entity, mesh] : scene->GetRegistry().view<Mesh>().each())
		{
			if (path == mesh.GetPath() && name == mesh.GetName())
			{
				m = mesh;
				break;
			}
		}
	}
}

/*
* namespace ImGui
*/

void ImGui::Inspect(Mesh& aMesh)
{
	auto& factory = SceneFactory::Get();

	if (BeginCombo("Filename", aMesh.GetPath().filename().string().c_str()))
	{
		for (auto& [path, scene] : factory.GetAssets())
		{
			bool selected = (path == aMesh.GetPath());
			if (Selectable(path.filename().string().c_str(), selected) && !selected)
			{
				aMesh = { path, aiMesh{} };
				break;
			}
		}
		EndCombo();
	}

	if (BeginCombo("Mesh Name", aMesh.GetName().data()))
	{
		if (auto scene = factory.GetAsset(aMesh.GetPath()))
		{
			for (auto [entity, mesh] : scene->GetRegistry().view<Mesh>().each())
			{
				bool selected = (mesh.GetName() == aMesh.GetName());
				if (Selectable(mesh.GetName().data(), selected) && !selected)
				{
					aMesh = mesh;
					break;
				}
			}
		}
		EndCombo();
	}

	Value("Vertices", aMesh.GetVertexCount());
	Value("Triangles", aMesh.GetIndexCount() / 3);

	auto& box = aMesh.GetBoundingBox();
	Text("Center: (%.2g, %.2g, %.2g)", box.Center.x, box.Center.y, box.Center.z);
	Text("Extents: (%.2g, %.2g, %.2g)", box.Extents.x, box.Extents.y, box.Extents.z);
}
