#include "pch.h"
#include "Renderer.h"
#include "ShaderCommon.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "Light.h"
#include "FullscreenPass.h"

Renderer::Renderer(unsigned aWidth, unsigned aHeight)
{
	ResizeBuffers(aWidth, aHeight);

	myMeshBuffer = { sizeof(MeshBuffer) };
	if (!myMeshBuffer)
		return;

	myMeshBuffer.VSSetBuffer(CBUFFER_SLOT_MESH);
	myMeshBuffer.PSSetBuffer(CBUFFER_SLOT_MESH);

	myLightBuffer = { sizeof(LightBuffer) };
	if (!myLightBuffer)
		return;
	
	myLightBuffer.PSSetBuffer(CBUFFER_SLOT_LIGHT);

	myEntityPixel = { DXGI_FORMAT_R32_UINT };
	if (!myEntityPixel)
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
}

void Renderer::ResizeBuffers(unsigned aWidth, unsigned aHeight)
{
	static constexpr std::array geometryFormats
	{
		DXGI_FORMAT_R32G32B32A32_FLOAT, // World position
		DXGI_FORMAT_R10G10B10A2_UNORM,  // Vertex normal
		DXGI_FORMAT_R10G10B10A2_UNORM,  // Pixel normal
		DXGI_FORMAT_R8G8B8A8_UNORM,		// Albedo
		DXGI_FORMAT_R8G8B8A8_UNORM,		// Metallic + Roughness + AO + [Unused]
		DXGI_FORMAT_R32_UINT,			// Entity
	};

	static constexpr std::array lightningFormats
	{
		DXGI_FORMAT_R32G32B32A32_FLOAT // Linear color space
	};

	myDepthBuffer = { aWidth, aHeight };
	myGeometryBuffer = { aWidth, aHeight, geometryFormats };
	myLightningBuffer = { aWidth, aHeight, lightningFormats };
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
	RenderLightning(aRegistry);
	RenderSkybox();
	TonemapAndGammaCorrect();
}

entt::entity Renderer::PickEntity(unsigned x, unsigned y)
{
	entt::entity entity{ entt::null };
	myEntityPixel.Pick(myGeometryBuffer.GetTexture(TEXTURE_SLOT_GBUFFER_ENTITY), x, y);
	myEntityPixel.Read(&entity, sizeof(entity));
	return entity;
}

Renderer::operator bool() const
{
	return
		myDepthBuffer && myGeometryBuffer && myLightningBuffer &&
		myMeshBuffer && myLightBuffer && myEntityPixel;
}

void Renderer::ClearBuffers()
{
	myDepthBuffer.Clear();
	myGeometryBuffer.Clear();
	myLightningBuffer.Clear();

	ScopedRenderTargets scopedTargets{ myGeometryBuffer.GetRenderTarget(TEXTURE_SLOT_GBUFFER_ENTITY) };
	FullscreenPass{ PIXEL_SHADER("PsUintMax.cso") }.Render();
}

void Renderer::RenderGeometry(entt::registry& aRegistry)
{
	if (aRegistry.empty())
		return;

	ScopedInputLayout scopedLayout{ typeid(BasicVertex) };
	ScopedShader scopedVs{ VERTEX_SHADER("VsBasic.cso") };
	ScopedShader scopedPs{ PIXEL_SHADER("PsGBuffer.cso") };
	ScopedRenderTargets scopedTargets{ myGeometryBuffer, myDepthBuffer };

	auto view = aRegistry.view<const Material::Ptr, const Mesh::Ptr, const Transform::Ptr>();
	for (auto [entity, material, mesh, transform] : view.each())
	{
		ScopedShaderResources scopedResources{ ShaderType::Pixel, 10, *material }; // TODO: turn hardcoded 10 into am acro
		
		mesh->SetBuffers();

		MeshBuffer buffer{};
		buffer.meshMatrix = transform->GetWorldMatrix();
		buffer.meshMatrixInverseTranspose = buffer.meshMatrix.Invert().Transpose();
		std::ranges::fill(buffer.meshEntity, std::to_underlying(entity));

		myMeshBuffer.WriteToBuffer(&buffer);

		DX11_CONTEXT->DrawIndexed(mesh->GetIndexCount(), 0, 0);
	}
}

void Renderer::RenderLightning(entt::registry& aRegistry)
{
	std::vector<DirectionalLight> dLights{};
	std::vector<PointLight> pLights{};

	auto view = aRegistry.view<const Light, const Transform::Ptr>();
	for (auto [entity, light, transform] : view.each())
	{
		if (!light.enabled)
			continue;

		const Matrix worldMatrix{ transform->GetWorldMatrix() };

		// todo: culling

		switch (light.GetType())
		{
		case LightType::Directional:
		{
			auto& dLight{ dLights.emplace_back(light.GetLight<DirectionalLight>()) };
			dLight.color.Premultiply();
			dLight.direction = Vector3::TransformNormal(dLight.direction, worldMatrix);
			dLight.direction.Normalize();
			break;
		}
		case LightType::Point:
		{
			auto& pLight{ pLights.emplace_back(light.GetLight<PointLight>()) };
			pLight.color.Premultiply();
			pLight.position = Vector3::Transform(pLight.position, worldMatrix);
			break;
		}
		}
	}

	ScopedShaderResources scopedResources{ ShaderType::Pixel, 0, myGeometryBuffer };  // TODO: turn hardcoded 0 into am acro
	ScopedRenderTargets scopedTargets{ myLightningBuffer };

	RenderDirectionalLights(dLights);
	RenderPointLights(pLights);
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

void Renderer::RenderDirectionalLights(std::span<const DirectionalLight> someLights)
{
	FullscreenPass lightPass{ PIXEL_SHADER("PsDirectionalLight.cso") };

	for (auto& light : someLights)
	{
		LightBuffer buffer{};
		buffer.lightColor = light.color;
		buffer.lightDirection = { light.direction.x, light.direction.y, light.direction.z, 0.f };
		myLightBuffer.WriteToBuffer(&buffer);

		lightPass.Render();
	}
}

void Renderer::RenderPointLights(std::span<const PointLight> someLights)
{
	FullscreenPass lightPass{ PIXEL_SHADER("PsPointLight.cso") };

	for (auto& light : someLights)
	{
		LightBuffer buffer{};
		buffer.lightColor = light.color;
		buffer.lightPosition = { light.position.x, light.position.y, light.position.z, 1.f };
		buffer.lightParameters = light.parameters;
		myLightBuffer.WriteToBuffer(&buffer);

		lightPass.Render();
	}
}

