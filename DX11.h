#pragma once

namespace DX11
{
	bool Create();
	void Destroy();

	RasterizerStatePtr GetRasterizerState(const D3D11_RASTERIZER_DESC&);
	SamplerStatePtr GetSamplerState(const D3D11_SAMPLER_DESC&);
	DepthStencilStatePtr GetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC&);
	BlendStatePtr GetBlendState(const D3D11_BLEND_DESC&);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();

	struct Scope
	{
		const bool ok;

		Scope() : ok{ Create() } {}
		~Scope() { Destroy(); }
	};
}

class ScopedTopology
{
public:
	ScopedTopology(D3D11_PRIMITIVE_TOPOLOGY);
	~ScopedTopology();

private:
	D3D11_PRIMITIVE_TOPOLOGY myTopology{};
};

