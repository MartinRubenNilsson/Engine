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
	SPECIALIZE_HASH(D3D11_SAMPLER_DESC)
	SPECIALIZE_HASH(D3D11_DEPTH_STENCIL_DESC)
	SPECIALIZE_HASH(D3D11_BLEND_DESC)

	SPECIALIZE_EQUAL_TO(D3D11_RASTERIZER_DESC)
	SPECIALIZE_EQUAL_TO(D3D11_SAMPLER_DESC)
	SPECIALIZE_EQUAL_TO(D3D11_DEPTH_STENCIL_DESC)
	SPECIALIZE_EQUAL_TO(D3D11_BLEND_DESC)
}

class StateFactory : public Singleton<StateFactory>
{
public:
	RasterizerStatePtr	 GetRasterizerState(const D3D11_RASTERIZER_DESC&);
	SamplerStatePtr		 GetSamplerState(const D3D11_SAMPLER_DESC&);
	DepthStencilStatePtr GetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC&);
	BlendStatePtr		 GetBlendState(const D3D11_BLEND_DESC&);

private:
	std::unordered_map<D3D11_RASTERIZER_DESC,	 RasterizerStatePtr>   myRasterizerStates;
	std::unordered_map<D3D11_SAMPLER_DESC,	     SamplerStatePtr>	   mySamplerStates;
	std::unordered_map<D3D11_DEPTH_STENCIL_DESC, DepthStencilStatePtr> myDepthStencilStates;
	std::unordered_map<D3D11_BLEND_DESC,		 BlendStatePtr>		   myBlendStates;
};

