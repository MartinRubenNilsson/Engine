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
* class ScopedResources
*/

ScopedResources::ScopedResources(UINT aStartSlot, std::span<ID3D11ShaderResourceView* const> someResources)
	: myStartSlot{ aStartSlot }
	, myPreviousResources{ someResources.size(), nullptr }
{
	assert(aStartSlot < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	assert(aStartSlot + someResources.size() < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
}

ScopedResources::~ScopedResources()
{
	for (ID3D11ShaderResourceView* resource : myPreviousResources)
	{
		if (resource)
			resource->Release();
	}
}

/*
* class ScopedResourcesPs
*/

ScopedResourcesPs::ScopedResourcesPs(UINT aStartSlot, std::span<ID3D11ShaderResourceView* const> someResources)
	: ScopedResources(aStartSlot, someResources)
{
	DX11_CONTEXT->PSGetShaderResources(myStartSlot, (UINT)myPreviousResources.size(), myPreviousResources.data());
	DX11_CONTEXT->PSSetShaderResources(myStartSlot, (UINT)someResources.size(), someResources.data());
}

ScopedResourcesPs::~ScopedResourcesPs()
{
	DX11_CONTEXT->PSSetShaderResources(myStartSlot, (UINT)myPreviousResources.size(), myPreviousResources.data());
}
