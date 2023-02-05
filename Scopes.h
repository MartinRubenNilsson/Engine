#pragma once

class Scope
{
protected:
	Scope() = default;

private:
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

class ScopedPsResources : Scope
{
public:
	ScopedPsResources(UINT aStartSlot, std::span<ID3D11ShaderResourceView* const> someResources);
	~ScopedPsResources();

	template <class T>
	ScopedPsResources(UINT aStartSlot, T& t)
		: ScopedPsResources(aStartSlot, t.GetShaderResources())
	{}

private:
	unsigned myStartSlot;
	std::vector<ID3D11ShaderResourceView*> myPreviousResources;
};
