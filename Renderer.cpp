#include "pch.h"
#include "Renderer.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"

Renderer::Renderer(unsigned aWidth, unsigned aHeight)
{
	if (!CreateBuffers(aWidth, aHeight))
		return;

	// Skybox
	{
		std::array<fs::path, 6> skyboxImagePaths
		{
			"cubemap/Sorsele/posx.jpg",
			"cubemap/Sorsele/negx.jpg",
			"cubemap/Sorsele/posy.jpg",
			"cubemap/Sorsele/negy.jpg",
			"cubemap/Sorsele/posz.jpg",
			"cubemap/Sorsele/negz.jpg",
		};

		mySkybox = { skyboxImagePaths };
		if (!mySkybox)
			return;
	}

	mySucceeded = true;
}

void Renderer::Render(entt::registry& aRegistry)
{
	if (!operator bool())
		return;

	D3D11_SAMPLER_DESC samplers[]
	{
		CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} },
	};

	ScopedSamplerStates scopedSamplers{ 0, samplers }; // todo: replace 0 with macro?

	ClearBuffers();
	RenderGeometry(aRegistry);
	RenderLightning();
	RenderSkybox();
	TonemapAndGammaCorrect();
}

bool Renderer::CreateBuffers(unsigned aWidth, unsigned aHeight)
{
	static constexpr std::array geometryFormats
	{
		DXGI_FORMAT_R32G32B32A32_FLOAT, // World position
		DXGI_FORMAT_R32G32B32A32_FLOAT, // Vertex normal
		DXGI_FORMAT_R32G32B32A32_FLOAT, // Pixel normal
		DXGI_FORMAT_R8G8B8A8_UNORM,		// Albedo
		DXGI_FORMAT_R8G8B8A8_UNORM,		// Metallic + Roughness + AO + [Unused]
		DXGI_FORMAT_R32_UINT,			// Entity
	};

	static constexpr std::array lightningFormat{ DXGI_FORMAT_R32G32B32A32_FLOAT };

	myDepthBuffer = { aWidth, aHeight };
	if (!myDepthBuffer)
		return false;

	myGeometryBuffer = { aWidth, aHeight, geometryFormats };
	if (!myGeometryBuffer)
		return false;

	myLightningBuffer = { aWidth, aHeight, lightningFormat };
	if (!myLightningBuffer)
		return false;

	return true;
}

void Renderer::ClearBuffers()
{
	myDepthBuffer.Clear();
	myGeometryBuffer.Clear();
	myLightningBuffer.Clear();
}

void Renderer::RenderGeometry(entt::registry& aRegistry)
{
	if (aRegistry.empty())
		return;

	ScopedInputLayout scopedLayout{ typeid(BasicVertex) };
	ScopedShader scopedVs{ VERTEX_SHADER("VsBasic.cso") };
	ScopedShader scopedPs{ PIXEL_SHADER("PsGBuffer.cso") };
	ScopedRenderTargets scopedTargets{ myGeometryBuffer, myDepthBuffer };

	auto view = aRegistry.view<Material::Ptr, Mesh::Ptr, Transform::Ptr>();
	for (auto [_, material, mesh, transform] : view.each())
	{
		ScopedShaderResources scopedResources{ ShaderType::Pixel, 10, *material }; // TODO: turn hardcoded 10 into am acro
		mesh->Draw(transform->GetWorldMatrix());
	}
}

void Renderer::RenderLightning()
{
	ScopedShaderResources scopedResources{ ShaderType::Pixel, 0, myGeometryBuffer };  // TODO: turn hardcoded 0 into am acro
	ScopedRenderTargets scopedTargets{ myLightningBuffer };

	FullscreenPass{ PIXEL_SHADER("PsPbr.cso") }.Render();
}

void Renderer::RenderSkybox()
{
	ScopedRenderTargets scopedTargets{ myLightningBuffer, myDepthBuffer };

	mySkybox.DrawSkybox();
}

void Renderer::TonemapAndGammaCorrect()
{
	static FullscreenPass passes[]
	{
		PIXEL_SHADER("PsTonemapAndGammaCorrect.cso"),
		PIXEL_SHADER("PsGBufferWorldPosition.cso"),
		PIXEL_SHADER("PsGBufferVertexNormal.cso"),
		PIXEL_SHADER("PsGBufferPixelNormal.cso"),
		PIXEL_SHADER("PsGBufferAlbedo.cso"),
		PIXEL_SHADER("PsGBufferMetalRoughAo.cso"),
		PIXEL_SHADER("PsGBufferEntity.cso"),
	};

	if (pass == 0)
	{
		ScopedShaderResources scopedResources{ ShaderType::Pixel, 0, myLightningBuffer };
		passes[pass].Render();
	}
	else if (pass < std::size(passes))
	{
		ScopedShaderResources scopedResources{ ShaderType::Pixel, 0, myGeometryBuffer };
		passes[pass].Render();
	}
}
