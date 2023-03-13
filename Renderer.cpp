#include "pch.h"
#include "Renderer.h"
#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "Light.h"
#include "FullscreenPass.h"
#include "Cubemap.h"

const char* RenderOutputToString(RenderOutput anOutput)
{
	static constexpr std::array strings
	{
		"Final",
		"Depth",
		"Normal",
		"Position",
		"Entity",
		"Access",
	};

	return strings.at(std::to_underlying(anOutput));
}

namespace
{
	/*
	* TODO: Move somewhere else
	*/

	auto AssembleMaterialResources(std::span<const Texture::Ptr> someTextures)
	{
		std::vector<ShaderResourcePtr> resources(MATERIAL_COUNT);

		for (const auto& texture : someTextures)
		{
			if (!texture)
				continue;

			switch (texture->GetType())
			{
			case TextureType::Albedo:
				resources.at(t_MaterialAlbedo - MATERIAL_BEGIN) = texture->GetResource();
				break;
			case TextureType::Normal:
				resources.at(t_MaterialNormal - MATERIAL_BEGIN) = texture->GetResource();
				break;
			case TextureType::Metallic:
				resources.at(t_MaterialMetallic - MATERIAL_BEGIN) = texture->GetResource();
				break;
			case TextureType::Roughness:
				resources.at(t_MaterialRoughness - MATERIAL_BEGIN) = texture->GetResource();
				break;
			}
		}

		return resources;
	}
}

/*
* class Renderer
*/

Renderer::Renderer(unsigned aWidth, unsigned aHeight)
	: myTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST }
	, mySamplers{ 0, GetSamplerDescs() }
	, myGaussianMap{ CreateGaussianMap() }, myIntegrationMap{ CreateIntegrationMap() }
	, myCBuffers{ sizeof(ImmutableBuffer), sizeof(CameraBuffer), sizeof(MeshBuffer), sizeof(LightBuffer) }
{
	if (!myGaussianMap)
		return;

	if (!myIntegrationMap)
		return;

	if (!std::ranges::all_of(myCBuffers, &ConstantBuffer::operator bool))
		return;

	{
		auto buffer{ ImmutableBuffer::Create() };
		myCBuffers.at(b_Immutable).Update(&buffer);
	}

	for (unsigned i = 0; i < myCBuffers.size(); ++i)
	{
		myCBuffers[i].VSSetBuffer(i);
		myCBuffers[i].GSSetBuffer(i);
		myCBuffers[i].PSSetBuffer(i);
	}

	if (!ResizeTextures(aWidth, aHeight))
		return;

	mySucceeded = true;
}

bool Renderer::ResizeTextures(unsigned aWidth, unsigned aHeight)
{
	myDepthBuffer = { aWidth, aHeight };
	if (!myDepthBuffer)
		return false;

	myRenderTextures.at(t_GBufferNormalDepth)	= { aWidth, aHeight, DXGI_FORMAT_R16G16B16A16_UNORM };
	myRenderTextures.at(t_GBufferAlbedo)		= { aWidth, aHeight, DXGI_FORMAT_R8G8B8A8_UNORM };
	myRenderTextures.at(t_GBufferMetalRoughAo)	= { aWidth, aHeight, DXGI_FORMAT_R8G8B8A8_UNORM };
	myRenderTextures.at(t_GBufferEntity)		= { aWidth, aHeight, DXGI_FORMAT_R32_UINT };
	myRenderTextures.at(t_AmbientAccessMap)		= { aWidth / 2, aHeight / 2, DXGI_FORMAT_R16_UNORM };
	myRenderTextures.at(t_BlurInputTexture)		= { aWidth / 2, aHeight / 2, DXGI_FORMAT_R16_UNORM };
	myRenderTextures.at(t_LightingTexture)		= { aWidth, aHeight, DXGI_FORMAT_R32G32B32A32_FLOAT };

	return std::ranges::all_of(myRenderTextures, &RenderTexture::operator bool);
}

void Renderer::SetCamera(const Camera& aCamera, const Matrix& aTransform)
{
	// Update constant buffer
	{
		CameraBuffer buffer{ aCamera.GetBuffer(USE_REVERSE_Z) };
		buffer.viewProj = aTransform.Invert() * buffer.viewProj;
		buffer.invViewProj = buffer.invViewProj * aTransform;
		Vector4::Transform(buffer.position, aTransform, buffer.position);

		myCBuffers.at(b_Camera).Update(&buffer);
	}

	// Update bounding frustum
	{
		BoundingFrustum::CreateFromMatrix(myFrustum, aCamera.GetProjectionMatrix() );
		myFrustum.Transform(myFrustum, aTransform);
	}
}

void Renderer::Render(const entt::registry& aRegistry)
{
	if (!operator bool())
		return;

	Clear();

	RenderGeometry(aRegistry);

	ScopedShaderResources scopedGBuffer{ ShaderType::Pixel, GBUFFER_BEGIN, GetGBufferResources() };

	if (settings.ssao)
		RenderSSAO();

	RenderLights(aRegistry);

	switch (output)
	{
	case RenderOutput::Final:
	{
		ScopedShaderResources scopedLightning{ ShaderType::Pixel, t_LightingTexture, myRenderTextures.at(t_LightingTexture) };
		FullscreenPass{ "PsTonemapAndGamma.cso" }.Render();
		break;
	}
	case RenderOutput::Depth:
	{
		ScopedShaderResources scopedResource{ ShaderType::Pixel, t_GBufferNormalDepth, myRenderTextures.at(t_GBufferNormalDepth) };
		FullscreenPass{ "PsGetDepth.cso" }.Render();
		break;
	}
	case RenderOutput::Normal:
	{
		ScopedShaderResources scopedResource{ ShaderType::Pixel, t_GBufferNormalDepth, myRenderTextures.at(t_GBufferNormalDepth) };
		FullscreenPass{ "PsGetNormal.cso" }.Render();
		break;
	}
	case RenderOutput::Position:
	{
		ScopedShaderResources scopedResource{ ShaderType::Pixel, t_GBufferNormalDepth, myRenderTextures.at(t_GBufferNormalDepth) };
		FullscreenPass{ "PsGetPosition.cso" }.Render();
		break;
	}
	case RenderOutput::Entity:
	{
		ScopedShaderResources scopedResource{ ShaderType::Pixel, t_GBufferEntity, myRenderTextures.at(t_GBufferEntity) };
		FullscreenPass{ "PsGetEntity.cso" }.Render();
		break;
	}
	case RenderOutput::Access:
	{
		ScopedShaderResources scopedResource{ ShaderType::Pixel, 0, myRenderTextures.at(t_AmbientAccessMap) };
		FullscreenPass{ "PsPointSample.cso" }.Render();
		break;
	}
	}
}

entt::entity Renderer::PickEntity(unsigned x, unsigned y)
{
	entt::entity entity{ entt::null };
	std::span span{ &entity, 1 };
	myRenderTextures.at(t_GBufferEntity).GetTexel(std::as_writable_bytes(span), x, y);
	return entity;
}

void Renderer::Clear()
{
	statistics = {};

	for (RenderTexture& texture : myRenderTextures)
		texture.Clear();

	myRenderTextures.at(t_GBufferNormalDepth).Clear({ 0.f, 0.f, 0.f, FAR_Z });
	myRenderTextures.at(t_AmbientAccessMap).Clear({ 1.f, 1.f, 1.f, 1.f });

	myDepthBuffer.Clear(FAR_Z);

	ScopedRenderTargets scopedTargets{ myRenderTextures.at(t_GBufferEntity) };
	FullscreenPass{ "PsClearEntity.cso" }.Render();
}

void Renderer::RenderGeometry(const entt::registry& aRegistry)
{
#if USE_REVERSE_Z
	CD3D11_DEPTH_STENCIL_DESC depthDesc{ CD3D11_DEFAULT{} };
	depthDesc.DepthFunc = D3D11_COMPARISON_GREATER;
	ScopedDepthStencilState scopedDepth{ depthDesc };
#endif

	ScopedInputLayout scopedLayout{ typeid(VsInBasic) };
	ScopedShader scopedVs{ "VsBasic.cso" };
	ScopedShader scopedPs{ "PsGBuffer.cso" };
	ScopedRenderTargets scopedTargets{ GetGBufferTargets(), myDepthBuffer };

	auto view = aRegistry.view<const Transform::Ptr, const Mesh::Ptr, const Material>();
	for (auto [entity, transform, mesh, material] : view.each())
	{
		const Matrix worldMatrix{ transform->GetWorldMatrix() };

		// Frustum culling
		{
			BoundingOrientedBox box{};
			BoundingOrientedBox::CreateFromBoundingBox(box, mesh->GetBoundingBox());
			box.Transform(box, worldMatrix);

			if (!myFrustum.Intersects(box))
				continue;
		}

		// Update constant buffer
		{
			MeshBuffer buffer{};
			buffer.matrix = worldMatrix;
			buffer.matrixInvTrans = worldMatrix.Invert().Transpose();
			std::ranges::fill(buffer.entity, std::to_underlying(entity));

			myCBuffers.at(b_Mesh).Update(&buffer);
		}

		ScopedShaderResources scopedResources
		{
			ShaderType::Pixel, MATERIAL_BEGIN,
			AssembleMaterialResources(material.GetTextures())
		};

		mesh->Draw();
		statistics.meshes++;
	}
}

void Renderer::RenderSSAO()
{
	auto& ambientMap{ myRenderTextures.at(t_AmbientAccessMap) };
	auto& blurTexture{ myRenderTextures.at(t_BlurInputTexture) };

	ScopedViewports scopedViewports{ ambientMap.GetViewport() };

	// Compute (noisy) ambient map
	{
		ScopedShaderResources scopedResource{ ShaderType::Pixel, t_GaussianMap, myGaussianMap };
		ScopedRenderTargets scopedTarget{ ambientMap };
		FullscreenPass{ "PsSSAO.cso" }.Render();
	}

	static constexpr size_t blurPassCount{ 4 };
	
	for (size_t i = 0; i < blurPassCount; ++i)
	{
		// Blur ambient map horizontally
		{
			ScopedShaderResources scopedResource{ ShaderType::Pixel, t_BlurInputTexture, ambientMap };
			ScopedRenderTargets scopedTarget{ blurTexture };
			FullscreenPass{ "PsBlurHorizontal.cso" }.Render();
		}

		// Blur ambient map vertically
		{
			ScopedShaderResources scopedResource{ ShaderType::Pixel, t_BlurInputTexture, blurTexture };
			ScopedRenderTargets scopedTarget{ ambientMap };
			FullscreenPass{ "PsBlurVertical.cso" }.Render();
		}
	}
}

void Renderer::RenderLights(const entt::registry& aRegistry)
{
	CD3D11_BLEND_DESC blendDesc{ CD3D11_DEFAULT{} };
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	ScopedRenderTargets scopedTargets{ myRenderTextures.at(t_LightingTexture) };
	ScopedBlendState scopedBlend{ blendDesc };

	{
		std::vector<LightBuffer> dLights{};
		std::vector<LightBuffer> pLights{};
		std::vector<LightBuffer> sLights{};

		auto view = aRegistry.view<const Light, const Transform::Ptr>();
		for (auto [entity, light, transform] : view.each())
		{
			if (!light.enabled)
				continue;

			const Matrix worldMatrix{ transform->GetWorldMatrix() }; // todo: remove scale

			// todo: culling

			LightBuffer buffer{ light.GetBuffer() };
			Vector4::Transform(buffer.position, worldMatrix, buffer.position);
			Vector4::Transform(buffer.direction, worldMatrix, buffer.direction);

			switch (light.GetType())
			{
			case LightType::Directional:
				dLights.push_back(buffer);
				break;
			case LightType::Point:
				pLights.push_back(buffer);
				break;
			case LightType::Spot:
				sLights.push_back(buffer);
				break;
			}
		}

		RenderDirectionalLights(dLights);
		RenderPointLights(pLights);
		RenderSpotLights(sLights);
	}

	if (auto cubemap = CubemapFactory::Get().GetAsset("cubemap/hdr/Newport_Loft_Ref.hdr"))
	{
		ScopedShaderResources scopedIntegMap{ ShaderType::Pixel, t_IntegrationMap, myIntegrationMap };
		ScopedShaderResources scopedCubemap{ ShaderType::Pixel, t_EnvironmentMap, cubemap->GetMaps() };

		{
			ScopedShaderResources scopedResource{ ShaderType::Pixel, t_AmbientAccessMap, myRenderTextures.at(t_AmbientAccessMap) };
			FullscreenPass{ "PsImageBasedLight.cso" }.Render();
		}

		RenderSkybox();
	}
}

void Renderer::RenderSkybox()
{
	CD3D11_DEPTH_STENCIL_DESC depthStencil{ CD3D11_DEFAULT{} };
#if USE_REVERSE_Z
	depthStencil.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
#else
	depthStencil.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
#endif

	ScopedPrimitiveTopology scopedTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };
	ScopedInputLayout scopedLayout{ typeid(EmptyVertex) };
	ScopedShader scopedVs{ "VsCubemap.cso" };
	ScopedShader scopedGs{ "GsSkybox.cso" };
	ScopedShader scopedPs{ "PsSkybox.cso" };
	ScopedRenderTargets scopedTargets{ myRenderTextures.at(t_LightingTexture), myDepthBuffer };
	ScopedDepthStencilState scopedDepthStencil{ depthStencil };

	DX11_CONTEXT->Draw(CUBEMAP_VERTEX_COUNT, 0);
}

void Renderer::RenderDirectionalLights(std::span<const LightBuffer> someLights)
{
	FullscreenPass pass{ "PsDirectionalLight.cso" };

	for (auto& light : someLights)
	{
		myCBuffers.at(b_Light).Update(&light);
		pass.Render();
		statistics.dLights++;
	}
}

void Renderer::RenderPointLights(std::span<const LightBuffer> someLights)
{
	FullscreenPass pass{ "PsPointLight.cso" };

	for (auto& light : someLights)
	{
		myCBuffers.at(b_Light).Update(&light);
		pass.Render();
		statistics.pLights++;
	}
}

void Renderer::RenderSpotLights(std::span<const LightBuffer> someLights)
{
	FullscreenPass pass{ "PsSpotLight.cso" };

	for (auto& light : someLights)
	{
		myCBuffers.at(b_Light).Update(&light);
		pass.Render();
		statistics.sLights++;
	}
}

std::vector<RenderTargetPtr> Renderer::GetGBufferTargets() const
{
	std::vector<RenderTargetPtr> targets{};
	for (size_t i = GBUFFER_BEGIN; i < GBUFFER_END; ++i)
		targets.emplace_back(myRenderTextures.at(i));
	return targets;
}

std::vector<ShaderResourcePtr> Renderer::GetGBufferResources() const
{
	std::vector<ShaderResourcePtr> resources{};
	for (size_t i = GBUFFER_BEGIN; i < GBUFFER_END; ++i)
		resources.emplace_back(myRenderTextures.at(i));
	return resources;
}

/*
* namespace ImGui
*/

void ImGui::InspectRenderer(Renderer& aRenderer)
{
	if (BeginCombo("Output", RenderOutputToString(aRenderer.output)))
	{
		for (int i = 0; i < std::to_underlying(RenderOutput::Count); ++i)
		{
			RenderOutput output{ i };
			if (Selectable(RenderOutputToString(output), aRenderer.output == output))
				aRenderer.output = output;
		}
		EndCombo();
	}

	Checkbox("SSAO", &aRenderer.settings.ssao);

	Value("Meshes", aRenderer.statistics.meshes);
	Value("Dir. Lights", aRenderer.statistics.dLights);
	Value("Point Lights", aRenderer.statistics.pLights);
	Value("Spot Lights", aRenderer.statistics.sLights);
}
