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

class ScopedRasterizerState : Scope
{
public:
	ScopedRasterizerState(const D3D11_RASTERIZER_DESC& aDesc);
	~ScopedRasterizerState();

private:
	D3D11_RASTERIZER_DESC myPreviousDesc;
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

class ScopedTargets : Scope
{
public:
	ScopedTargets(std::span<ID3D11RenderTargetView* const> someTargets, ID3D11DepthStencilView* aDepthStencil = nullptr);
	~ScopedTargets();

	template <class T>
	ScopedTargets(T& t)
		: ScopedTargets(t.GetRenderTargets())
	{}

	template <class T, class U>
	ScopedTargets(T& t, U& u)
		: ScopedTargets(t.GetRenderTargets(), u.GetDepthStencil())
	{}

private:
	std::vector<ID3D11RenderTargetView*> myPreviousTargets;
	ID3D11DepthStencilView* myPreviousDepthStencil;
};

class ScopedResources : Scope
{
protected:
	ScopedResources(UINT aStartSlot, std::span<ID3D11ShaderResourceView* const> someResources);
	~ScopedResources();

	unsigned myStartSlot;
	std::vector<ID3D11ShaderResourceView*> myPreviousResources;
};

class ScopedResourcesPs : ScopedResources
{
public:
	ScopedResourcesPs(UINT aStartSlot, std::span<ID3D11ShaderResourceView* const> someResources);
	~ScopedResourcesPs();

	template <class T>
	ScopedResourcesPs(UINT aStartSlot, T& t)
		: ScopedResourcesPs(aStartSlot, t.GetShaderResources())
	{}
};
