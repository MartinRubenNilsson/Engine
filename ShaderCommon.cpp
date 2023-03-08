#include "pch.h"
#include "ShaderCommon.h"

std::span<const D3D11_SAMPLER_DESC> GetSamplerDescs()
{
	static std::array<D3D11_SAMPLER_DESC, SamplerCount> samplers{};
	samplers.fill(CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} });

	samplers[s_PointSampler].Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

	samplers[s_TrilinearSampler].Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	samplers[s_GaussianSampler].Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplers[s_GaussianSampler].AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplers[s_GaussianSampler].AddressV = D3D11_TEXTURE_ADDRESS_WRAP;

	return samplers;
}
