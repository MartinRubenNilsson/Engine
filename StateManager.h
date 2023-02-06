#pragma once

#define SPECIALIZE_HASH(DESC) \
template <> \
struct hash<DESC> \
{ \
	size_t operator()(const DESC& aDesc) const \
	{ \
		return hash<string_view>{}({ reinterpret_cast<const char*>(&aDesc), sizeof(DESC) }); \
	} \
}; \

#define SPECIALIZE_EQUAL_TO(DESC) \
template <> \
struct equal_to<DESC> \
{ \
	bool operator()(const DESC& lhs, const DESC& rhs) const \
	{ \
		return memcmp(&lhs, &rhs, sizeof(DESC)) == 0; \
	} \
}; \

namespace std
{
	SPECIALIZE_HASH(D3D11_RASTERIZER_DESC)
	SPECIALIZE_HASH(D3D11_DEPTH_STENCIL_DESC)
	SPECIALIZE_HASH(D3D11_BLEND_DESC)

	SPECIALIZE_EQUAL_TO(D3D11_RASTERIZER_DESC)
	SPECIALIZE_EQUAL_TO(D3D11_DEPTH_STENCIL_DESC)
	SPECIALIZE_EQUAL_TO(D3D11_BLEND_DESC)
}

class StateManager : public Singleton<StateManager>
{
public:
	void SetRasterizerState(const D3D11_RASTERIZER_DESC&);
	void GetRasterizerState(D3D11_RASTERIZER_DESC&) const;
	void SetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC&, UINT aStencilRef);
	void GetDepthStencilState(D3D11_DEPTH_STENCIL_DESC&, UINT& aStencilRef) const;
	void SetBlendState(const D3D11_BLEND_DESC&, const FLOAT aBlendFactor[4], UINT aSampleMask);
	void GetBlendState(D3D11_BLEND_DESC&, FLOAT aBlendFactor[4], UINT& aSampleMask) const;

private:
	std::unordered_map<D3D11_RASTERIZER_DESC, ComPtr<ID3D11RasterizerState>> myRasterizerStates;
	std::unordered_map<D3D11_DEPTH_STENCIL_DESC, ComPtr<ID3D11DepthStencilState>> myDepthStencilStates;
	std::unordered_map<D3D11_BLEND_DESC, ComPtr<ID3D11BlendState>> myBlendStates;
};

