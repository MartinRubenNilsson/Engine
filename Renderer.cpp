#include "pch.h"
#include "Renderer.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"

Renderer::Renderer(unsigned aWidth, unsigned aHeight)
{
	// Geometry buffer
	{
		static constexpr std::array formats
		{
			DXGI_FORMAT_R32G32B32A32_FLOAT, // World position
			DXGI_FORMAT_R32G32B32A32_FLOAT, // Vertex normal
			DXGI_FORMAT_R32G32B32A32_FLOAT, // Pixel normal
			DXGI_FORMAT_R8G8B8A8_UNORM,		// Albedo
			DXGI_FORMAT_R8G8B8A8_UNORM,		// Metallic + Roughness + AO + [Unused]
			DXGI_FORMAT_R32_UINT,			// Entity
		};

		myGeometryBuffer = { aWidth, aHeight, formats };
		if (!myGeometryBuffer)
			return;
	}

	// Output buffer
	{
		static constexpr std::array formats
		{
			DXGI_FORMAT_R32G32B32A32_FLOAT
		};

		myOutputBuffer = { aWidth, aHeight, formats };
		if (!myOutputBuffer)
			return;
	}

	// Depth buffer
	myDepthBuffer = { aWidth, aHeight };
	if (!myDepthBuffer)
		return;

	// Fullscreen passes
	myFullscreenPasses =
	{
		PIXEL_SHADER("PsPbr.cso"),
		PIXEL_SHADER("PsGBufferWorldPosition.cso"),
		PIXEL_SHADER("PsGBufferVertexNormal.cso"),
		PIXEL_SHADER("PsGBufferPixelNormal.cso"),
		PIXEL_SHADER("PsGBufferAlbedo.cso"),
		PIXEL_SHADER("PsGBufferMetalRoughAo.cso"),
		PIXEL_SHADER("PsGBufferEntity.cso"),
	};
	if (!std::ranges::all_of(myFullscreenPasses, &FullscreenPass::operator bool))
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
	myWidth = aWidth;
	myHeight = aHeight;
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

void Renderer::ClearBuffers()
{
	myGeometryBuffer.Clear();
	myOutputBuffer.Clear();
	myDepthBuffer.Clear();
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
	ScopedRenderTargets scopedTargets{ myOutputBuffer };
	myFullscreenPasses[pass].Render();
}

void Renderer::RenderSkybox()
{
	ScopedRenderTargets scopedTargets{ myOutputBuffer, myDepthBuffer };
	mySkybox.DrawSkybox();
}

void Renderer::TonemapAndGammaCorrect()
{
	ScopedShaderResources scopedResources{ ShaderType::Pixel, 0, myOutputBuffer };
	FullscreenPass{ PIXEL_SHADER("PsTonemapAndGammaCorrect.cso") }.Render();
}
