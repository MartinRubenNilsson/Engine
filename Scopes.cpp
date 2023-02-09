#include "pch.h"
#include "Scopes.h"
#include "InputLayoutManager.h"
#include "StateManager.h"

/*
* class ScopedPrimitiveTopology
*/

ScopedPrimitiveTopology::ScopedPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY aTopology)
	: myPreviousTopology{ D3D_PRIMITIVE_TOPOLOGY_UNDEFINED }
{
	DX11_CONTEXT->IAGetPrimitiveTopology(&myPreviousTopology);
	DX11_CONTEXT->IASetPrimitiveTopology(aTopology);
}

ScopedPrimitiveTopology::~ScopedPrimitiveTopology()
{
	DX11_CONTEXT->IASetPrimitiveTopology(myPreviousTopology);
}

/*
* class ScopedInputLayout
*/

ScopedInputLayout::ScopedInputLayout(std::type_index aVertexType)
{
	DX11_CONTEXT->IAGetInputLayout(&myPreviousLayout);
	InputLayoutManager::Get().SetInputLayout(aVertexType);
}

ScopedInputLayout::~ScopedInputLayout()
{
	DX11_CONTEXT->IASetInputLayout(myPreviousLayout.Get());
}

/*
* class ScopedShader
*/

ScopedShader::ScopedShader(std::shared_ptr<const Shader> aShader)
{
	if (!aShader)
		return;

	if (auto vs = std::dynamic_pointer_cast<const VertexShader>(aShader))
		myPreviousShader = std::make_shared<VertexShader>(*vs);
	else if (auto ps = std::dynamic_pointer_cast<const PixelShader>(aShader))
		myPreviousShader = std::make_shared<PixelShader>(*ps);
	else
		assert(false);

	myPreviousShader->GetShader();
	aShader->SetShader();
}

ScopedShader::~ScopedShader()
{
	if (myPreviousShader)
		myPreviousShader->SetShader();
}

/*
* class ScopedRasterizerState
*/

ScopedRasterizerState::ScopedRasterizerState(const D3D11_RASTERIZER_DESC& aDesc)
	: myPreviousDesc{ CD3D11_RASTERIZER_DESC{ CD3D11_DEFAULT{} } }
{
	StateManager::Get().GetRasterizerState(myPreviousDesc);
	StateManager::Get().SetRasterizerState(aDesc);
}

ScopedRasterizerState::~ScopedRasterizerState()
{
	StateManager::Get().SetRasterizerState(myPreviousDesc);
}

/*
* class ScopedSamplerStates
*/

ScopedSamplerStates::ScopedSamplerStates(UINT aStartSlot, std::span<const D3D11_SAMPLER_DESC> someDescs)
	: myStartSlot{ aStartSlot }
	, myPreviousDescs{ someDescs.size(), CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} } }
{
	StateManager::Get().GetSamplerStates(myStartSlot, myPreviousDescs);
	StateManager::Get().SetSamplerStates(aStartSlot, someDescs);
}

ScopedSamplerStates::~ScopedSamplerStates()
{
	StateManager::Get().SetSamplerStates(myStartSlot, myPreviousDescs);
}

/*
* class ScopedDepthStencilState
*/

ScopedDepthStencilState::ScopedDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& aDesc, UINT aStencilRef)
	: myPreviousDesc{ CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} } }
	, myPreviousStencilRef{}
{
	StateManager::Get().GetDepthStencilState(myPreviousDesc, myPreviousStencilRef);
	StateManager::Get().SetDepthStencilState(aDesc, aStencilRef);
}

ScopedDepthStencilState::~ScopedDepthStencilState()
{
	StateManager::Get().SetDepthStencilState(myPreviousDesc, myPreviousStencilRef);
}

/*
* class ScopedRenderTargets
*/

ScopedRenderTargets::ScopedRenderTargets(std::span<ID3D11RenderTargetView* const> someTargets, DepthStencil aDepthStencil)
	: myPreviousTargets{ someTargets.size(), nullptr }
	, myPreviousDepthStencil{}
{
	assert(someTargets.size() <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

	DX11_CONTEXT->OMGetRenderTargets((UINT)myPreviousTargets.size(), myPreviousTargets.data(), &myPreviousDepthStencil);
	DX11_CONTEXT->OMSetRenderTargets((UINT)someTargets.size(), someTargets.data(), aDepthStencil.Get());
}

ScopedRenderTargets::~ScopedRenderTargets()
{
	DX11_CONTEXT->OMSetRenderTargets((UINT)myPreviousTargets.size(), myPreviousTargets.data(), myPreviousDepthStencil.Get());

	for (ID3D11RenderTargetView* target : myPreviousTargets)
	{
		if (target)
			target->Release();
	}
}

/*
* class ScopedViewports
*/

ScopedViewports::ScopedViewports(std::span<const D3D11_VIEWPORT> someViewports)
	: myPreviousViewports{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE }
{
	assert(someViewports.size() <= D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE);

	UINT previousViewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	DX11_CONTEXT->RSGetViewports(&previousViewportCount, myPreviousViewports.data());
	myPreviousViewports.resize(previousViewportCount);

	DX11_CONTEXT->RSSetViewports((UINT)someViewports.size(), someViewports.data());
}

ScopedViewports::~ScopedViewports()
{
	DX11_CONTEXT->RSSetViewports((UINT)myPreviousViewports.size(), myPreviousViewports.data());
}

/*
* class ScopedShaderResources
*/

ScopedShaderResources::ScopedShaderResources(ShaderStage aStage, UINT aStartSlot, std::span<ID3D11ShaderResourceView* const> someResources)
	: myStage{ aStage }
	, myStartSlot{ aStartSlot }
	, myPreviousResources{ someResources.size(), nullptr }
{
	assert(aStartSlot < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	assert(aStartSlot + someResources.size() <= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);

	switch (aStage)
	{
	case ShaderStage::Vertex:
		DX11_CONTEXT->VSGetShaderResources(myStartSlot, (UINT)myPreviousResources.size(), myPreviousResources.data());
		DX11_CONTEXT->VSSetShaderResources(aStartSlot, (UINT)someResources.size(), someResources.data());
		break;
	case ShaderStage::Pixel:
		DX11_CONTEXT->PSGetShaderResources(myStartSlot, (UINT)myPreviousResources.size(), myPreviousResources.data());
		DX11_CONTEXT->PSSetShaderResources(aStartSlot, (UINT)someResources.size(), someResources.data());
		break;
	}
}

ScopedShaderResources::~ScopedShaderResources()
{
	switch (myStage)
	{
	case ShaderStage::Vertex:
		DX11_CONTEXT->VSSetShaderResources(myStartSlot, (UINT)myPreviousResources.size(), myPreviousResources.data());
		break;
	case ShaderStage::Pixel:
		DX11_CONTEXT->PSSetShaderResources(myStartSlot, (UINT)myPreviousResources.size(), myPreviousResources.data());
		break;
	}

	for (ID3D11ShaderResourceView* resource : myPreviousResources)
	{
		if (resource)
			resource->Release();
	}
}
