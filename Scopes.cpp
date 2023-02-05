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
* class ScopedTargets
*/

ScopedTargets::ScopedTargets(std::span<ID3D11RenderTargetView* const> someTargets, ID3D11DepthStencilView* aDepthStencil)
	: myPreviousTargets{ someTargets.size(), nullptr }
	, myPreviousDepthStencil{ nullptr }
{
	assert(someTargets.size() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

	DX11_CONTEXT->OMGetRenderTargets((UINT)myPreviousTargets.size(), myPreviousTargets.data(), &myPreviousDepthStencil);
	DX11_CONTEXT->OMSetRenderTargets((UINT)someTargets.size(), someTargets.data(), aDepthStencil);
}

ScopedTargets::~ScopedTargets()
{
	DX11_CONTEXT->OMSetRenderTargets((UINT)myPreviousTargets.size(), myPreviousTargets.data(), myPreviousDepthStencil);

	for (ID3D11RenderTargetView* target : myPreviousTargets)
	{
		if (target)
			target->Release();
	}

	if (myPreviousDepthStencil)
		myPreviousDepthStencil->Release();
}

/*
* class ScopedResources
*/

ScopedResources::ScopedResources(UINT aStartSlot, std::span<ID3D11ShaderResourceView* const> someResources)
	: myStartSlot{ aStartSlot }
	, myPreviousResources{ someResources.size(), nullptr }
{
	assert(aStartSlot < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	assert(aStartSlot + someResources.size() <= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
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
