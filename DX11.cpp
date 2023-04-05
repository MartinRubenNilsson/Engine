#include "pch.h"
#include "DX11.h"
#pragma comment(lib, "DXGI") 
#pragma comment(lib, "D3D11")

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

namespace
{
	ID3D11Device* theDevice = nullptr;
	ID3D11DeviceContext* theContext = nullptr;

	std::unordered_map<D3D11_RASTERIZER_DESC, RasterizerStatePtr> theRasterizerStates{};
	std::unordered_map<D3D11_SAMPLER_DESC, SamplerStatePtr> theSamplerStates{};
	std::unordered_map<D3D11_DEPTH_STENCIL_DESC, DepthStencilStatePtr> theDepthStencilStates{};
	std::unordered_map<D3D11_BLEND_DESC, BlendStatePtr> theBlendStates{};
}

/*
* namespace DX11
*/

bool DX11::Create()
{
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	std::array levels
	{
		D3D_FEATURE_LEVEL_11_1,
	};

	HRESULT result = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		levels.data(),
		(UINT)levels.size(),
		D3D11_SDK_VERSION,
		&theDevice,
		NULL,
		&theContext
	);

	return SUCCEEDED(result);
}

void DX11::Destroy()
{
	theRasterizerStates.clear();
	theSamplerStates.clear();
	theDepthStencilStates.clear();
	theBlendStates.clear();

	if (theContext) { theContext->Release(); theContext = nullptr; }
	if (theDevice) { theDevice->Release(); theDevice = nullptr; }
}

RasterizerStatePtr DX11::GetRasterizerState(const D3D11_RASTERIZER_DESC& aDesc)
{
	auto& state = theRasterizerStates[aDesc];
	if (!state)
	{
		assert(theRasterizerStates.size() <= D3D11_REQ_RASTERIZER_OBJECT_COUNT_PER_DEVICE);
		theDevice->CreateRasterizerState(&aDesc, &state);
	}
	return state;
}

SamplerStatePtr DX11::GetSamplerState(const D3D11_SAMPLER_DESC& aDesc)
{
	auto& state = theSamplerStates[aDesc];
	if (!state)
	{
		assert(theSamplerStates.size() <= D3D11_REQ_SAMPLER_OBJECT_COUNT_PER_DEVICE);
		theDevice->CreateSamplerState(&aDesc, &state);
	}
	return state;
}

DepthStencilStatePtr DX11::GetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& aDesc)
{
	auto& state = theDepthStencilStates[aDesc];
	if (!state)
	{
		assert(theDepthStencilStates.size() <= D3D11_REQ_DEPTH_STENCIL_OBJECT_COUNT_PER_DEVICE);
		theDevice->CreateDepthStencilState(&aDesc, &state);
	}
	return state;
}

BlendStatePtr DX11::GetBlendState(const D3D11_BLEND_DESC& aDesc)
{
	auto& state = theBlendStates[aDesc];
	if (!state)
	{
		assert(theDepthStencilStates.size() <= D3D11_REQ_BLEND_OBJECT_COUNT_PER_DEVICE);
		theDevice->CreateBlendState(&aDesc, &state);
	}
	return state;
}

ID3D11Device* DX11::GetDevice()
{
	return theDevice;
}

ID3D11DeviceContext* DX11::GetContext()
{
	return theContext;
}
