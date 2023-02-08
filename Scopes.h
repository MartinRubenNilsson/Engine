#pragma once

class Scope
{
protected:
	Scope() = default;
	Scope(const Scope&) = delete;
	Scope& operator=(const Scope&) = delete;
	Scope(Scope&&) = delete;
	Scope& operator=(Scope&&) = delete;
};

class ScopedPrimitiveTopology : Scope
{
public:
	ScopedPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY aTopology);
	~ScopedPrimitiveTopology();

private:
	D3D11_PRIMITIVE_TOPOLOGY myPreviousTopology;
};

class ScopedShader : Scope
{
public:
	ScopedShader(std::shared_ptr<const Shader> aShader);
	~ScopedShader();

private:
	std::shared_ptr<Shader> myPreviousShader;
};

class ScopedRasterizerState : Scope
{
public:
	ScopedRasterizerState(const D3D11_RASTERIZER_DESC& aDesc);
	~ScopedRasterizerState();

private:
	D3D11_RASTERIZER_DESC myPreviousDesc;
};

class ScopedSamplerStates : Scope
{
public:
	ScopedSamplerStates(UINT aStartSlot, std::span<const D3D11_SAMPLER_DESC> someDescs);
	~ScopedSamplerStates();

private:
	UINT myStartSlot;
	std::vector<D3D11_SAMPLER_DESC> myPreviousDescs;
};

class ScopedDepthStencilState : Scope
{
public:
	ScopedDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& aDesc, UINT aStencilRef);
	~ScopedDepthStencilState();

private:
	D3D11_DEPTH_STENCIL_DESC myPreviousDesc;
	UINT myPreviousStencilRef;
};

class ScopedRenderTargets : Scope
{
public:
	ScopedRenderTargets(std::span<ID3D11RenderTargetView* const> someTargets, ID3D11DepthStencilView* aDepthStencil = nullptr);
	~ScopedRenderTargets();

private:
	std::vector<ID3D11RenderTargetView*> myPreviousTargets;
	ID3D11DepthStencilView* myPreviousDepthStencil;
};

class ScopedViewports : Scope
{
public:
	ScopedViewports(std::span<const D3D11_VIEWPORT> someViewports);
	~ScopedViewports();

private:
	std::vector<D3D11_VIEWPORT> myPreviousViewports;
};

class ScopedResources : Scope
{
protected:
	ScopedResources(UINT aStartSlot, std::span<ID3D11ShaderResourceView* const> someResources);
	~ScopedResources();

	unsigned myStartSlot;
	std::vector<ID3D11ShaderResourceView*> myPreviousResources;
};

class ScopedPixelShaderResources : ScopedResources
{
public:
	ScopedPixelShaderResources(UINT aStartSlot, std::span<ID3D11ShaderResourceView* const> someResources);
	~ScopedPixelShaderResources();
};
