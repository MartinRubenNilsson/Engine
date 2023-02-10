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

class ScopedInputLayout : Scope
{
public:
	ScopedInputLayout(std::type_index aVertexType);
	~ScopedInputLayout();

private:
	ComPtr<ID3D11InputLayout> myPreviousLayout;
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
	ScopedRenderTargets(std::span<const RenderTargetPtr> someTargets, DepthStencilPtr aDepthStencil = nullptr);
	~ScopedRenderTargets();

private:
	static constexpr UINT ourCount = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;

	RenderTargetPtr myPreviousTargets[ourCount];
	DepthStencilPtr myPreviousDepthStencil;
};

class ScopedViewports : Scope
{
public:
	ScopedViewports(std::span<const D3D11_VIEWPORT> someViewports);
	~ScopedViewports();

private:
	std::vector<D3D11_VIEWPORT> myPreviousViewports;
};

enum class ShaderStage
{
	Vertex,
	Pixel
};

class ScopedShaderResources : Scope
{
public:
	ScopedShaderResources(ShaderStage aStage, UINT aStartSlot, std::span<const ShaderResourcePtr> someResources);
	~ScopedShaderResources();

private:
	UINT myStartSlot;
	std::vector<ShaderResourcePtr> myPreviousResources;
	decltype(&ID3D11DeviceContext::VSSetShaderResources) mySetter;
	decltype(&ID3D11DeviceContext::VSGetShaderResources) myGetter;
};

