#include "pch.h"
#include "Renderer.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"

Renderer::Renderer(HWND hWnd)
{
	myBackBuffer = { hWnd };
	if (!myBackBuffer)
		return;

	const unsigned width = myBackBuffer.GetWidth();
	const unsigned height = myBackBuffer.GetHeight();

	myDepthBuffer = { width, height };
	if (!myDepthBuffer)
		return;

	// GBuffer
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

		myGeometryBuffer = { width, height, formats };
		if (!myGeometryBuffer)
			return;
	}

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
}

void Renderer::Render(entt::registry& aRegistry)
{
	ClearBuffers();
	RenderGeometry(aRegistry);
	RenderLightning();
	RenderSkybox();
}

void Renderer::ClearBuffers()
{
	myBackBuffer.Clear();
	myDepthBuffer.Clear();
	myGeometryBuffer.Clear();
}

void Renderer::RenderGeometry(entt::registry& aRegistry)
{
	if (aRegistry.empty())
		return;

	ScopedInputLayout layout{ typeid(BasicVertex) };
	ScopedShader vertexShader{ VERTEX_SHADER("VsBasic.cso") };
	ScopedShader pixelShader{ PIXEL_SHADER("PsGBuffer.cso") };
	ScopedRenderTargets targets{ myGeometryBuffer, myDepthBuffer };

	auto view = aRegistry.view<Material::Ptr, Mesh::Ptr, Transform::Ptr>();
	for (auto [_, material, mesh, transform] : view.each())
	{
		ScopedShaderResources resources{ ShaderType::Pixel, 10, *material };
		mesh->Draw(transform->GetWorldMatrix());
	}
}

void Renderer::RenderLightning()
{
	ScopedShaderResources resources{ ShaderType::Pixel, 0, myGeometryBuffer };
	myFullscreenPasses[pass].Render();
}

void Renderer::RenderSkybox()
{
	ScopedRenderTargets targets{ myBackBuffer, myDepthBuffer };
	mySkybox.DrawSkybox();
}
