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

	switch (aShader->GetType())
	{
	case ShaderType::Vertex:
		myPreviousShader = std::make_shared<VertexShader>();
		break;
	case ShaderType::Pixel:
		myPreviousShader = std::make_shared<PixelShader>();
		break;
	default:
		assert(false);
		break;
	}

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

ScopedRenderTargets::ScopedRenderTargets(std::span<const RenderTargetPtr> someTargets, DepthStencilPtr aDepthStencil)
	: myPreviousTargets{}
	, myPreviousDepthStencil{}
{
	assert(someTargets.size() <= ourCount);

	{
		ID3D11RenderTargetView* views[ourCount]{};
		DX11_CONTEXT->OMGetRenderTargets(ourCount, views, myPreviousDepthStencil.ReleaseAndGetAddressOf());
		std::ranges::copy(views, myPreviousTargets);
	}
	
	{
		ID3D11RenderTargetView* views[ourCount]{};
		std::ranges::transform(someTargets, views, &RenderTargetPtr::Get);
		DX11_CONTEXT->OMSetRenderTargets(ourCount, views, aDepthStencil.Get());
	}
}

ScopedRenderTargets::~ScopedRenderTargets()
{
	ID3D11RenderTargetView* views[ourCount]{};
	std::ranges::transform(myPreviousTargets, views, &RenderTargetPtr::Get);
	DX11_CONTEXT->OMSetRenderTargets(ourCount, views, myPreviousDepthStencil.Get());
}

/*
* class ScopedViewports
*/

ScopedViewports::ScopedViewports(std::span<const D3D11_VIEWPORT> someViewports)
{
	UINT prevCount{ (UINT)myPreviousViewports.size() };
	UINT currCount{ std::min(prevCount, (UINT)someViewports.size()) };
	DX11_CONTEXT->RSGetViewports(&prevCount, myPreviousViewports.data());
	DX11_CONTEXT->RSSetViewports(currCount, someViewports.data());
}

ScopedViewports::~ScopedViewports()
{
	DX11_CONTEXT->RSSetViewports((UINT)myPreviousViewports.size(), myPreviousViewports.data());
}

/*
* class ScopedShaderResources
*/

ScopedShaderResources::ScopedShaderResources(ShaderStage aStage, UINT aStartSlot, std::span<const ShaderResourcePtr> someResources)
	: myStartSlot{ aStartSlot }
	, myPreviousResources{ someResources.size() }
	, mySetter{}
	, myGetter{}
{
	assert(aStartSlot < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	assert(aStartSlot + someResources.size() <= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);

	switch (aStage)
	{
	case ShaderStage::Vertex:
		mySetter = &ID3D11DeviceContext::VSSetShaderResources;
		myGetter = &ID3D11DeviceContext::VSGetShaderResources;
		break;
	case ShaderStage::Pixel:
		mySetter = &ID3D11DeviceContext::PSSetShaderResources;
		myGetter = &ID3D11DeviceContext::PSGetShaderResources;
		break;
	default:
		assert(false);
	}

	{
		std::vector<ID3D11ShaderResourceView*> views{ myPreviousResources.size() };
		std::invoke(myGetter, DX11_CONTEXT, myStartSlot, (UINT)views.size(), views.data());
		std::ranges::copy(views, myPreviousResources.begin());
	}

	{
		std::vector<ID3D11ShaderResourceView*> views{ someResources.size() };
		std::ranges::transform(someResources, views.begin(), &ShaderResourcePtr::Get);
		std::invoke(mySetter, DX11_CONTEXT, aStartSlot, (UINT)views.size(), views.data());
	}
}

ScopedShaderResources::~ScopedShaderResources()
{
	std::vector<ID3D11ShaderResourceView*> views{ myPreviousResources.size() };
	std::ranges::transform(myPreviousResources, views.begin(), &ShaderResourcePtr::Get);
	std::invoke(mySetter, DX11_CONTEXT, myStartSlot, (UINT)views.size(), views.data());
}
