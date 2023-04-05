#include "pch.h"
#include "Scopes.h"
#include "DX11.h"
#include "InputLayoutFactory.h"

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
* class ScopedCurrentPath
*/

ScopedCurrentPath::ScopedCurrentPath(const fs::path& aPath)
	: myPath{ fs::current_path() }
{
	fs::current_path(aPath);
}

ScopedCurrentPath::~ScopedCurrentPath()
{
	fs::current_path(myPath);
}



/*
* class ScopedLayout
*/

ScopedLayout::ScopedLayout(std::type_index aVertexType)
{
	DX11::GetContext()->IAGetInputLayout(&myLayout);
	DX11::GetContext()->IASetInputLayout(InputLayoutFactory::Get().GetInputLayout(aVertexType).Get());
}

ScopedLayout::~ScopedLayout()
{
	DX11::GetContext()->IASetInputLayout(myLayout.Get());
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
	DX11::GetContext()->RSGetState(&myRasterizerState);
	DX11::GetContext()->RSSetState(DX11::GetRasterizerState(aDesc).Get());
}

ScopedRasterizer::~ScopedRasterizer()
{
	DX11::GetContext()->RSSetState(myRasterizerState.Get());
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
			| std::views::transform([](auto& desc) { return DX11::GetSamplerState(desc).Get(); })
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
	
	DX11::GetContext()->PSGetSamplers(myStartSlot, (UINT)myStates.size(), myStates.data());
	DX11::GetContext()->PSSetSamplers(aStartSlot, (UINT)someStates.size(), someStates.data());
}

ScopedSamplers::~ScopedSamplers()
{
	DX11::GetContext()->PSSetSamplers(myStartSlot, (UINT)myStates.size(), myStates.data());
	ReleaseAll(std::span{ myStates });
}

/*
* class ScopedDepthStencil
*/

ScopedDepthStencil::ScopedDepthStencil(const D3D11_DEPTH_STENCIL_DESC& aDesc, UINT aStencilRef)
{
	DX11::GetContext()->OMGetDepthStencilState(&myState, &myStencilRef);
	DX11::GetContext()->OMSetDepthStencilState(DX11::GetDepthStencilState(aDesc).Get(), aStencilRef);
}

ScopedDepthStencil::~ScopedDepthStencil()
{
	DX11::GetContext()->OMSetDepthStencilState(myState.Get(), myStencilRef);
}

/*
* class ScopedBlend
*/

ScopedBlend::ScopedBlend(const D3D11_BLEND_DESC& aDesc, const FLOAT aBlendFactor[4], UINT aSampleMask)
{
	DX11::GetContext()->OMGetBlendState(&myState, myBlendFactor, &mySampleMask);
	DX11::GetContext()->OMSetBlendState(DX11::GetBlendState(aDesc).Get(), aBlendFactor, aSampleMask);
}

ScopedBlend::~ScopedBlend()
{
	DX11::GetContext()->OMSetBlendState(myState.Get(), myBlendFactor, mySampleMask);
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

	DX11::GetContext()->OMGetRenderTargets((UINT)myTargets.size(), myTargets.data(), &myDepthStencil);
	DX11::GetContext()->OMSetRenderTargets((UINT)someTargets.size(), someTargets.data(), aDepthStencil.Get());
}

ScopedTargets::~ScopedTargets()
{
	DX11::GetContext()->OMSetRenderTargets((UINT)myTargets.size(), myTargets.data(), myDepthStencil.Get());
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
	DX11::GetContext()->RSGetViewports(&count, myViewports.data());
	DX11::GetContext()->RSSetViewports((UINT)someViewports.size(), someViewports.data());
}

ScopedViewports::~ScopedViewports()
{
	DX11::GetContext()->RSSetViewports((UINT)myViewports.size(), myViewports.data());
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

	std::invoke(theGetters.at(std::to_underlying(myType)), DX11::GetContext(), myStartSlot, (UINT)myResources.size(), myResources.data());
	std::invoke(theSetters.at(std::to_underlying(aType)), DX11::GetContext(), aStartSlot, (UINT)someResources.size(), someResources.data());
}

ScopedResources::~ScopedResources()
{
	std::invoke(theSetters.at(std::to_underlying(myType)), DX11::GetContext(), myStartSlot, (UINT)myResources.size(), myResources.data());
	ReleaseAll(std::span{ myResources });
}
