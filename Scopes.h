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
	RasterizerStatePtr myRasterizerState{};
};

class ScopedSamplerStates : Scope
{
public:
	ScopedSamplerStates(UINT aStartSlot, const D3D11_SAMPLER_DESC& aDesc);
	ScopedSamplerStates(UINT aStartSlot, std::span<const D3D11_SAMPLER_DESC> someDescs);
	~ScopedSamplerStates();

private:
	ScopedSamplerStates(UINT aStartSlot, const std::vector<ID3D11SamplerState*>& someStates);

	UINT myStartSlot{};
	std::vector<ID3D11SamplerState*> myStates{};
};

class ScopedDepthStencilState : Scope
{
public:
	ScopedDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& aDesc, UINT aStencilRef = 0u);
	~ScopedDepthStencilState();

private:
	DepthStencilStatePtr myState{};
	UINT myStencilRef{};
};

class ScopedBlendState : Scope
{
public:
	ScopedBlendState(const D3D11_BLEND_DESC& aDesc, const FLOAT aBlendFactor[4] = NULL, UINT aSampleMask = 0xffffffff);
	~ScopedBlendState();

private:
	BlendStatePtr myState{};
	FLOAT myBlendFactor[4]{};
	UINT mySampleMask{};
};

class ScopedRenderTargets : Scope
{
public:
	ScopedRenderTargets(RenderTargetPtr aTarget, DepthStencilPtr aDepthStencil = nullptr);
	ScopedRenderTargets(std::span<const RenderTargetPtr> someTargets, DepthStencilPtr aDepthStencil = nullptr);
	~ScopedRenderTargets();

private:
	ScopedRenderTargets(const std::vector<ID3D11RenderTargetView*>& someTargets, DepthStencilPtr aDepthStencil);

	std::array<ID3D11RenderTargetView*, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> myTargets{};
	DepthStencilPtr myDepthStencil{};
};

class ScopedViewports : Scope
{
public:
	ScopedViewports(const D3D11_VIEWPORT& aViewport);
	ScopedViewports(std::span<const D3D11_VIEWPORT> someViewports);
	~ScopedViewports();

private:
	std::array<D3D11_VIEWPORT, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE> myViewports{};
};

class ScopedShaderResources : Scope
{
public:
	ScopedShaderResources(ShaderType aType, UINT aStartSlot, ShaderResourcePtr aResource);
	ScopedShaderResources(ShaderType aType, UINT aStartSlot, std::span<const ShaderResourcePtr> someResources);
	~ScopedShaderResources();

private:
	ScopedShaderResources(ShaderType aType, UINT aStartSlot, const std::vector<ID3D11ShaderResourceView*>& someResources);

	ShaderType myType{};
	UINT myStartSlot{};
	std::vector<ID3D11ShaderResourceView*> myResources;
};

