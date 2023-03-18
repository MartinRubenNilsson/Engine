#pragma once

class Scope
{
protected:
	Scope() = default;
	~Scope() = default;

	Scope(const Scope&) = delete;
	Scope& operator=(const Scope&) = delete;
	Scope(Scope&&) = delete;
	Scope& operator=(Scope&&) = delete;
};

class ScopedCurrentPath : Scope
{
public:
	ScopedCurrentPath(const fs::path&);
	~ScopedCurrentPath();

private:
	fs::path myPath{};
};

class ScopedTopology : Scope
{
public:
	ScopedTopology(D3D11_PRIMITIVE_TOPOLOGY);
	~ScopedTopology();

private:
	D3D11_PRIMITIVE_TOPOLOGY myTopology{};
};

class ScopedLayout : Scope
{
public:
	ScopedLayout(std::type_index aVertexType);
	~ScopedLayout();

private:
	InputLayoutPtr myLayout{};
};

class ScopedShader : Scope
{
public:
	ScopedShader(const fs::path& aPath);
	~ScopedShader();

private:
	ShaderVariant myShader{};
};

class ScopedRasterizer : Scope
{
public:
	ScopedRasterizer(const D3D11_RASTERIZER_DESC&);
	~ScopedRasterizer();

private:
	RasterizerStatePtr myRasterizerState{};
};

class ScopedSamplers : Scope
{
public:
	ScopedSamplers(UINT aStartSlot, std::initializer_list<D3D11_SAMPLER_DESC>);
	ScopedSamplers(UINT aStartSlot, std::span<const D3D11_SAMPLER_DESC>);
	~ScopedSamplers();

private:
	ScopedSamplers(UINT aStartSlot, const std::vector<ID3D11SamplerState*>&);

	UINT myStartSlot{};
	std::vector<ID3D11SamplerState*> myStates{};
};

class ScopedDepthStencil : Scope
{
public:
	ScopedDepthStencil(const D3D11_DEPTH_STENCIL_DESC&, UINT aStencilRef = 0u);
	~ScopedDepthStencil();

private:
	DepthStencilStatePtr myState{};
	UINT myStencilRef{};
};

class ScopedBlend : Scope
{
public:
	ScopedBlend(const D3D11_BLEND_DESC& aDesc, const FLOAT aBlendFactor[4] = NULL, UINT aSampleMask = 0xffffffff);
	~ScopedBlend();

private:
	BlendStatePtr myState{};
	FLOAT myBlendFactor[4]{};
	UINT mySampleMask{};
};

class ScopedTargets : Scope
{
public:
	ScopedTargets(std::initializer_list<RenderTargetPtr>, DepthStencilPtr aDepthStencil = nullptr);
	ScopedTargets(std::span<const RenderTargetPtr>, DepthStencilPtr aDepthStencil = nullptr);
	~ScopedTargets();

private:
	ScopedTargets(const std::vector<ID3D11RenderTargetView*>&, DepthStencilPtr);

	std::array<ID3D11RenderTargetView*, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> myTargets{};
	DepthStencilPtr myDepthStencil{};
};

class ScopedViewports : Scope
{
public:
	ScopedViewports(std::initializer_list<D3D11_VIEWPORT>);
	ScopedViewports(std::span<const D3D11_VIEWPORT>);
	~ScopedViewports();

private:
	std::array<D3D11_VIEWPORT, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE> myViewports{};
};

class ScopedResources : Scope
{
public:
	ScopedResources(ShaderType, UINT aStartSlot, std::initializer_list<ShaderResourcePtr>);
	ScopedResources(ShaderType, UINT aStartSlot, std::span<const ShaderResourcePtr>);
	~ScopedResources();

private:
	ScopedResources(ShaderType, UINT aStartSlot, const std::vector<ID3D11ShaderResourceView*>&);

	ShaderType myType{};
	UINT myStartSlot{};
	std::vector<ID3D11ShaderResourceView*> myResources;
};

