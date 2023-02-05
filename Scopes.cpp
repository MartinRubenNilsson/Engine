#include "pch.h"
#include "Scopes.h"

/*
* class ScopedTopology
*/

ScopedTopology::ScopedTopology(D3D11_PRIMITIVE_TOPOLOGY aTopology)
{
	DX11_CONTEXT->IAGetPrimitiveTopology(&myPreviousTopology);
	DX11_CONTEXT->IASetPrimitiveTopology(aTopology);
}

ScopedTopology::~ScopedTopology()
{
	DX11_CONTEXT->IASetPrimitiveTopology(myPreviousTopology);
}

/*
* class ScopedResourcesPs
*/

ScopedResourcesPs::ScopedResourcesPs(UINT aStartSlot, std::span<ID3D11ShaderResourceView* const> someResources)
{
	assert(aStartSlot < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	assert(aStartSlot + someResources.size() < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);

	myStartSlot = aStartSlot;
	myPreviousResources.resize(someResources.size(), nullptr);

	DX11_CONTEXT->PSGetShaderResources(myStartSlot, (UINT)myPreviousResources.size(), myPreviousResources.data());
	DX11_CONTEXT->PSSetShaderResources(myStartSlot, (UINT)someResources.size(), someResources.data());
}

ScopedResourcesPs::~ScopedResourcesPs()
{
	DX11_CONTEXT->PSSetShaderResources(myStartSlot, (UINT)myPreviousResources.size(), myPreviousResources.data());

	for (auto resource : myPreviousResources)
	{
		if (resource)
			resource->Release();
	}
}
