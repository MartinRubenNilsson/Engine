#pragma once

struct EmptyVertex
{
	static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
	{
		{ "SV_VertexID", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	static constexpr std::string_view Shader = "VsFullscreen.cso";
};

struct BasicVertex
{
	Vector3 position;
	Vector3 normal;
	Vector2 uv;

	static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	static constexpr std::string_view Shader = "VsBasic.cso";
};

