#include "pch.h"
#include "Scopes.h"
#include "InputLayoutFactory.h"
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
* class ScopedTopology
*/

ScopedTopology::ScopedTopology(D3D11_PRIMITIVE_TOPOLOGY aTopology)
{
	DX11_CONTEXT->IAGetPrimitiveTopology(&myTopology);
	DX11_CONTEXT->IASetPrimitiveTopology(aTopology);
}

ScopedTopology::~ScopedTopology()
{
	DX11_CONTEXT->IASetPrimitiveTopology(myTopology);
}

/*
* class ScopedLayout
*/

ScopedLayout::ScopedLayout(std::type_index aVertexType)
{
	DX11_CONTEXT->IAGetInputLayout(&myLayout);
	DX11_CONTEXT->IASetInputLayout(InputLayoutFactory::Get().GetInputLayout(aVertexType).Get());
}

ScopedLayout::~ScopedLayout()
{
	DX11_CONTEXT->IASetInputLayout(myLayout.Get());
}

/*
* class ScopedShader
*/

ScopedShader::ScopedShader(const fs::path& aPath)
{
	if (auto shader{ ShaderFactory::Get().GetAsset(aPath) })
	{
		EmplaceShader(shader->GetType(), myShader);
		GetShader(myShader);
		shader->SetShader();
	}
}

ScopedShader::~ScopedShader()
{
	SetShader(myShader);
}

/*
* class ScopedRasterizer
*/

ScopedRasterizer::ScopedRasterizer(const D3D11_RASTERIZER_DESC& aDesc)
{
	DX11_CONTEXT->RSGetState(&myRasterizerState);
	DX11_CONTEXT->RSSetState(StateFactory::Get().GetRasterizerState(aDesc).Get());
}

ScopedRasterizer::~ScopedRasterizer()
{
	DX11_CONTEXT->RSSetState(myRasterizerState.Get());
}

/*
* class ScopedSamplers
*/

ScopedSamplers::ScopedSamplers(UINT aStartSlot, std::initializer_list<D3D11_SAMPLER_DESC> someDescs)
	: ScopedSamplers{ aStartSlot, std::span{ someDescs } }
{
}

ScopedSamplers::ScopedSamplers(UINT aStartSlot, std::span<const D3D11_SAMPLER_DESC> someDescs)
	: ScopedSamplers{
		aStartSlot,
		someDescs
			| std::views::transform([](auto& desc) { return StateFactory::Get().GetSamplerState(desc).Get(); })
			| std::ranges::to<std::vector>()
	}
{
}

ScopedSamplers::ScopedSamplers(UINT aStartSlot, const std::vector<ID3D11SamplerState*>& someStates)
	: myStartSlot{ aStartSlot }
	, myStates{ someStates.size(), nullptr }
{
	assert(aStartSlot < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
	assert(aStartSlot + someStates.size() <= D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
	
	DX11_CONTEXT->PSGetSamplers(myStartSlot, (UINT)myStates.size(), myStates.data());
	DX11_CONTEXT->PSSetSamplers(aStartSlot, (UINT)someStates.size(), someStates.data());
}

ScopedSamplers::~ScopedSamplers()
{
	DX11_CONTEXT->PSSetSamplers(myStartSlot, (UINT)myStates.size(), myStates.data());
	ReleaseAll(std::span{ myStates });
}

/*
* class ScopedDepthStencil
*/

ScopedDepthStencil::ScopedDepthStencil(const D3D11_DEPTH_STENCIL_DESC& aDesc, UINT aStencilRef)
{
	DX11_CONTEXT->OMGetDepthStencilState(&myState, &myStencilRef);
	DX11_CONTEXT->OMSetDepthStencilState(StateFactory::Get().GetDepthStencilState(aDesc).Get(), aStencilRef);
}

ScopedDepthStencil::~ScopedDepthStencil()
{
	DX11_CONTEXT->OMSetDepthStencilState(myState.Get(), myStencilRef);
}

/*
* class ScopedBlend
*/

ScopedBlend::ScopedBlend(const D3D11_BLEND_DESC& aDesc, const FLOAT aBlendFactor[4], UINT aSampleMask)
{
	DX11_CONTEXT->OMGetBlendState(&myState, myBlendFactor, &mySampleMask);
	DX11_CONTEXT->OMSetBlendState(StateFactory::Get().GetBlendState(aDesc).Get(), aBlendFactor, aSampleMask);
}

ScopedBlend::~ScopedBlend()
{
	DX11_CONTEXT->OMSetBlendState(myState.Get(), myBlendFactor, mySampleMask);
}

/*
* class ScopedTargets
*/

ScopedTargets::ScopedTargets(std::initializer_list<RenderTargetPtr> someTargets, DepthStencilPtr aDepthStencil)
	: ScopedTargets(std::span{ someTargets }, aDepthStencil)
{
}

ScopedTargets::ScopedTargets(std::span<const RenderTargetPtr> someTargets, DepthStencilPtr aDepthStencil)
	: ScopedTargets{
		someTargets | std::views::transform(&RenderTargetPtr::Get) | std::ranges::to<std::vector>(),
		aDepthStencil
	}
{
}

ScopedTargets::ScopedTargets(const std::vector<ID3D11RenderTargetView*>& someTargets, DepthStencilPtr aDepthStencil)
{
	assert(someTargets.size() <= myTargets.size());

	DX11_CONTEXT->OMGetRenderTargets((UINT)myTargets.size(), myTargets.data(), &myDepthStencil);
	DX11_CONTEXT->OMSetRenderTargets((UINT)someTargets.size(), someTargets.data(), aDepthStencil.Get());
}

ScopedTargets::~ScopedTargets()
{
	DX11_CONTEXT->OMSetRenderTargets((UINT)myTargets.size(), myTargets.data(), myDepthStencil.Get());
	ReleaseAll(std::span{ myTargets });
}

/*
* class ScopedViewports
*/

ScopedViewports::ScopedViewports(std::initializer_list<D3D11_VIEWPORT> someViewports)
	: ScopedViewports(std::span{ someViewports })
{
}

ScopedViewports::ScopedViewports(std::span<const D3D11_VIEWPORT> someViewports)
{
	assert(someViewports.size() <= myViewports.size());

	UINT count{ (UINT)myViewports.size() };
	DX11_CONTEXT->RSGetViewports(&count, myViewports.data());
	DX11_CONTEXT->RSSetViewports((UINT)someViewports.size(), someViewports.data());
}

ScopedViewports::~ScopedViewports()
{
	DX11_CONTEXT->RSSetViewports((UINT)myViewports.size(), myViewports.data());
}

/*
* class ScopedResources
*/

namespace
{
	constexpr std::array theSetters
	{
		&ID3D11DeviceContext::PSSetShaderResources,
		&ID3D11DeviceContext::VSSetShaderResources,
		&ID3D11DeviceContext::GSSetShaderResources,
		&ID3D11DeviceContext::HSSetShaderResources,
		&ID3D11DeviceContext::DSSetShaderResources,
		&ID3D11DeviceContext::CSSetShaderResources,
	};

	constexpr std::array theGetters
	{
		&ID3D11DeviceContext::PSGetShaderResources,
		&ID3D11DeviceContext::VSGetShaderResources,
		&ID3D11DeviceContext::GSGetShaderResources,
		&ID3D11DeviceContext::HSGetShaderResources,
		&ID3D11DeviceContext::DSGetShaderResources,
		&ID3D11DeviceContext::CSGetShaderResources,
	};
}

ScopedResources::ScopedResources(ShaderType aType, UINT aStartSlot, std::initializer_list<ShaderResourcePtr> someResources)
	: ScopedResources(aType, aStartSlot, std::span{ someResources })
{
}

ScopedResources::ScopedResources(ShaderType aType, UINT aStartSlot, std::span<const ShaderResourcePtr> someResources)
	: ScopedResources{
		aType,
		aStartSlot,
		someResources | std::views::transform(&ShaderResourcePtr::Get) | std::ranges::to<std::vector>()
	}
{
}

ScopedResources::ScopedResources(ShaderType aType, UINT aStartSlot, const std::vector<ID3D11ShaderResourceView*>& someResources)
	: myType{ aType }
	, myStartSlot{ aStartSlot }
	, myResources{ someResources.size() }
{
	assert(aStartSlot < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	assert(aStartSlot + someResources.size() <= D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);

	std::invoke(theGetters.at(std::to_underlying(myType)), DX11_CONTEXT, myStartSlot, (UINT)myResources.size(), myResources.data());
	std::invoke(theSetters.at(std::to_underlying(aType)), DX11_CONTEXT, aStartSlot, (UINT)someResources.size(), someResources.data());
}

ScopedResources::~ScopedResources()
{
	std::invoke(theSetters.at(std::to_underlying(myType)), DX11_CONTEXT, myStartSlot, (UINT)myResources.size(), myResources.data());
	ReleaseAll(std::span{ myResources });
}

