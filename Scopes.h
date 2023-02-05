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

class ScopedTopology : Scope
{
public:
	ScopedTopology(D3D11_PRIMITIVE_TOPOLOGY aTopology);
	~ScopedTopology();

private:
	D3D11_PRIMITIVE_TOPOLOGY myPreviousTopology;
};

class ScopedResourcesPs : Scope
{
public:
	ScopedResourcesPs(UINT aStartSlot, std::span<ID3D11ShaderResourceView* const> someResources);
	~ScopedResourcesPs();

	template <class T>
	ScopedResourcesPs(UINT aStartSlot, T& t)
		: ScopedResourcesPs(aStartSlot, t.GetShaderResources())
	{}

private:
	unsigned myStartSlot;
	std::vector<ID3D11ShaderResourceView*> myPreviousResources;
};
