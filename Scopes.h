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
	D3D11_PRIMITIVE_TOPOLOGY myPreviousTopology{ D3D_PRIMITIVE_TOPOLOGY_UNDEFINED };
};

class ScopedInputLayout : Scope
{
public:
	ScopedInputLayout(std::type_index aVertexType);
	~ScopedInputLayout();

private:
	ComPtr<ID3D11InputLayout> myPreviousLayout{};
};

class ScopedShader : Scope
{
public:
	ScopedShader(std::shared_ptr<const Shader> aShader);
	~ScopedShader();

private:
	std::shared_ptr<Shader> myPreviousShader{};
};

class ScopedRasterizerState : Scope
{
public:
	ScopedRasterizerState(const D3D11_RASTERIZER_DESC& aDesc);
	~ScopedRasterizerState();

private:
	D3D11_RASTERIZER_DESC myPreviousDesc{ CD3D11_RASTERIZER_DESC{ CD3D11_DEFAULT{} } };
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
	D3D11_DEPTH_STENCIL_DESC myPreviousDesc{ CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} } };
	UINT myPreviousStencilRef{};
};

class ScopedRenderTargets : Scope
{
public:
	ScopedRenderTargets(std::span<const RenderTargetPtr> someTargets, DepthStencilPtr aDepthStencil = nullptr);
	~ScopedRenderTargets();

private:
	static constexpr UINT ourCount = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;

	std::array<RenderTargetPtr, ourCount> myPreviousTargets{};
	DepthStencilPtr myPreviousDepthStencil{};
};

class ScopedViewports : Scope
{
public:
	ScopedViewports(const D3D11_VIEWPORT& aViewport);
	ScopedViewports(std::span<const D3D11_VIEWPORT> someViewports);
	~ScopedViewports();

private:
	static constexpr UINT ourCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

	std::array<D3D11_VIEWPORT, ourCount> myPreviousViewports{};
};

class ScopedShaderResources : Scope
{
public:
	ScopedShaderResources(ShaderType aType, UINT aStartSlot, std::span<const ShaderResourcePtr> someResources);
	~ScopedShaderResources();

private:
	UINT myStartSlot;
	std::vector<ShaderResourcePtr> myPreviousResources;
	decltype(&ID3D11DeviceContext::VSSetShaderResources) mySetter;
	decltype(&ID3D11DeviceContext::VSGetShaderResources) myGetter;
};

