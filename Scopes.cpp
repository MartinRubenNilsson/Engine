#include "pch.h"
#include "Scopes.h"
#include "InputLayoutManager.h"
#include "StateFactory.h"

namespace
{
	template <class T, size_t Extent>
	void ReleaseAll(std::span<T*, Extent> somePtrs)
	{
		for (T* ptr : somePtrs)
		{
			if (ptr)
				ptr->Release();
		}
	}
}

/*
* class ScopedPrimitiveTopology
*/

ScopedPrimitiveTopology::ScopedPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY aTopology)
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
{
	DX11_CONTEXT->RSGetState(&myRasterizerState);
	DX11_CONTEXT->RSSetState(StateFactory::Get().GetRasterizerState(aDesc).Get());
}

ScopedRasterizerState::~ScopedRasterizerState()
{
	DX11_CONTEXT->RSSetState(myRasterizerState.Get());
}

/*
* class ScopedSamplerStates
*/

ScopedSamplerStates::ScopedSamplerStates(UINT aStartSlot, std::span<const D3D11_SAMPLER_DESC> someDescs)
	: ScopedSamplerStates{
		aStartSlot,
		someDescs
			| std::views::transform([](auto& desc) { return StateFactory::Get().GetSamplerState(desc).Get(); })
			| std::ranges::to<std::vector>()
	}
{
}

ScopedSamplerStates::ScopedSamplerStates(UINT aStartSlot, const std::vector<ID3D11SamplerState*>& someStates)
	: myStartSlot{ aStartSlot }
	, myStates{ someStates.size(), nullptr }
{
	assert(aStartSlot < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
	assert(aStartSlot + someStates.size() <= D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
	
	DX11_CONTEXT->PSGetSamplers(myStartSlot, (UINT)myStates.size(), myStates.data());
	DX11_CONTEXT->PSSetSamplers(aStartSlot, (UINT)someStates.size(), someStates.data());
}

ScopedSamplerStates::~ScopedSamplerStates()
{
	DX11_CONTEXT->PSSetSamplers(myStartSlot, (UINT)myStates.size(), myStates.data());
	ReleaseAll(std::span(myStates));
}

/*
* class ScopedDepthStencilState
*/

ScopedDepthStencilState::ScopedDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& aDesc, UINT aStencilRef)
{
	DX11_CONTEXT->OMGetDepthStencilState(&myState, &myStencilRef);
	DX11_CONTEXT->OMSetDepthStencilState(StateFactory::Get().GetDepthStencilState(aDesc).Get(), aStencilRef);
}

ScopedDepthStencilState::~ScopedDepthStencilState()
{
	DX11_CONTEXT->OMSetDepthStencilState(myState.Get(), myStencilRef);
}

/*
* class ScopedBlendState
*/

ScopedBlendState::ScopedBlendState(const D3D11_BLEND_DESC& aDesc, const FLOAT aBlendFactor[4], UINT aSampleMask)
{
	DX11_CONTEXT->OMGetBlendState(&myState, myBlendFactor, &mySampleMask);
	DX11_CONTEXT->OMSetBlendState(StateFactory::Get().GetBlendState(aDesc).Get(), aBlendFactor, aSampleMask);
}

ScopedBlendState::~ScopedBlendState()
{
	DX11_CONTEXT->OMSetBlendState(myState.Get(), myBlendFactor, mySampleMask);
}

/*
* class ScopedRenderTargets
*/

ScopedRenderTargets::ScopedRenderTargets(RenderTargetPtr aTarget, DepthStencilPtr aDepthStencil)
	: ScopedRenderTargets({ std::addressof(aTarget), 1 }, aDepthStencil)
{
}

ScopedRenderTargets::ScopedRenderTargets(std::span<const RenderTargetPtr> someTargets, DepthStencilPtr aDepthStencil)
	: ScopedRenderTargets{
		someTargets | std::views::transform(&RenderTargetPtr::Get) | std::ranges::to<std::vector>(),
		aDepthStencil
	}
{
}

ScopedRenderTargets::ScopedRenderTargets(const std::vector<ID3D11RenderTargetView*>& someTargets, DepthStencilPtr aDepthStencil)
{
	assert(someTargets.size() <= myTargets.size());

	DX11_CONTEXT->OMGetRenderTargets((UINT)myTargets.size(), myTargets.data(), &myDepthStencil);
	DX11_CONTEXT->OMSetRenderTargets((UINT)someTargets.size(), someTargets.data(), aDepthStencil.Get());
}

ScopedRenderTargets::~ScopedRenderTargets()
{
	DX11_CONTEXT->OMSetRenderTargets((UINT)myTargets.size(), myTargets.data(), myDepthStencil.Get());
	ReleaseAll(std::span(myTargets));
}

/*
* class ScopedViewports
*/

ScopedViewports::ScopedViewports(const D3D11_VIEWPORT& aViewport)
	: ScopedViewports({ &aViewport, 1 })
{
}

ScopedViewports::ScopedViewports(std::span<const D3D11_VIEWPORT> someViewports)
{
	UINT prevCount{ ourCount };
	UINT currCount{ std::min(ourCount, (UINT)someViewports.size()) };

	DX11_CONTEXT->RSGetViewports(&prevCount, myPreviousViewports.data());
	DX11_CONTEXT->RSSetViewports(currCount, someViewports.data());
}

ScopedViewports::~ScopedViewports()
{
	DX11_CONTEXT->RSSetViewports(ourCount, myPreviousViewports.data());
}

/*
* class ScopedShaderResources
*/

ScopedShaderResources::ScopedShaderResources(ShaderType aType, UINT aStartSlot, std::span<const ShaderResourcePtr> someResources)
	: myStartSlot{ aStartSlot }
	, myPreviousResources{ someResources.size() }
	, mySetter{}
	, myGetter{}
{
	assert(aStartSlot < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	assert(aStartSlot + someResources.size() <= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);

	switch (aType)
	{
	case ShaderType::Vertex:
		mySetter = &ID3D11DeviceContext::VSSetShaderResources;
		myGetter = &ID3D11DeviceContext::VSGetShaderResources;
		break;
	case ShaderType::Pixel:
		mySetter = &ID3D11DeviceContext::PSSetShaderResources;
		myGetter = &ID3D11DeviceContext::PSGetShaderResources;
		break;
	default:
		assert(false);
	}

	// BUGGY

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
