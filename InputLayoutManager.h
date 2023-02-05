#pragma once

/*
* Vertex types
*/

struct BasicVertex
{
	Vector3 position;
	Vector3 normal;
};

struct FullscreenVertex
{
	// none
};

/*
* Vertex traits
*/

template <class VertexType>
struct VertexTraits;

template <>
struct VertexTraits<BasicVertex>
{
	static constexpr D3D11_INPUT_ELEMENT_DESC elements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	static constexpr char shader[] = "VsBasic.cso";
};
template <>
struct VertexTraits<FullscreenVertex>
{
	static constexpr D3D11_INPUT_ELEMENT_DESC elements[] =
	{
		{ "SV_VertexID", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	static constexpr char shader[] = "VsFullscreen.cso";
};


/*
* class InputLayoutManager
*/

class InputLayoutManager : public Singleton<InputLayoutManager>
{
public:
	template <class VertexType>
	bool RegisterInputLayout();

	void SetInputLayout(std::type_index aVertexType) const;

private:
	bool CreateInputLayout(
		std::type_index aVertexType,
		std::span<const D3D11_INPUT_ELEMENT_DESC> someElements,
		const fs::path& aVertexShaderPath
	);

	std::unordered_map<std::type_index, ComPtr<ID3D11InputLayout>> myInputLayouts;
};

template<class VertexType>
inline bool InputLayoutManager::RegisterInputLayout()
{
	using VT = VertexTraits<VertexType>;
	return CreateInputLayout(typeid(VertexType), VT::elements, VT::shader);
}

#define DX11_SET_INPUT_LAYOUT(aVertexType) InputLayoutManager::Get().SetInputLayout(aVertexType)