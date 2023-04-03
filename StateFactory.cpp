#include "pch.h"
#include "StateFactory.h"

RasterizerStatePtr StateFactory::GetRasterizerState(const D3D11_RASTERIZER_DESC& aDesc)
{
	auto& state = myRasterizerStates[aDesc];
	if (!state)
	{
		assert(myRasterizerStates.size() <= D3D11_REQ_RASTERIZER_OBJECT_COUNT_PER_DEVICE);
		DX11::GetDevice()->CreateRasterizerState(&aDesc, &state);
	}
	return state;
}

SamplerStatePtr StateFactory::GetSamplerState(const D3D11_SAMPLER_DESC& aDesc)
{
	auto& state = mySamplerStates[aDesc];
	if (!state)
	{
		assert(mySamplerStates.size() <= D3D11_REQ_SAMPLER_OBJECT_COUNT_PER_DEVICE);
		DX11::GetDevice()->CreateSamplerState(&aDesc, &state);
	}
	return state;
}

DepthStencilStatePtr StateFactory::GetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& aDesc)
{
	auto& state = myDepthStencilStates[aDesc];
	if (!state)
	{
		assert(myDepthStencilStates.size() <= D3D11_REQ_DEPTH_STENCIL_OBJECT_COUNT_PER_DEVICE);
		DX11::GetDevice()->CreateDepthStencilState(&aDesc, &state);
	}
	return state;
}

BlendStatePtr StateFactory::GetBlendState(const D3D11_BLEND_DESC& aDesc)
{
	auto& state = myBlendStates[aDesc];
	if (!state)
	{
		assert(myDepthStencilStates.size() <= D3D11_REQ_BLEND_OBJECT_COUNT_PER_DEVICE);
		DX11::GetDevice()->CreateBlendState(&aDesc, &state);
	}
	return state;
}

