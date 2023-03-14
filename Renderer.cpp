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

/*
* class Renderer
*/

Renderer::Renderer(unsigned aWidth, unsigned aHeight)
{
	myCBuffers[b_Immutable] = { sizeof(ImmutableBuffer) };
	myCBuffers[b_Camera]	= { sizeof(CameraBuffer) };
	myCBuffers[b_Mesh]		= { sizeof(MeshBuffer) };
	myCBuffers[b_Light]		= { sizeof(LightBuffer) };

	for (auto& [slot, buffer] : myCBuffers)
	{
		if (!buffer)
			return;

		buffer.VSSetBuffer(slot);
		buffer.GSSetBuffer(slot);
		buffer.PSSetBuffer(slot);
	}

	{
		auto buffer{ ImmutableBuffer::Create() };
		myCBuffers.at(b_Immutable).Update(&buffer);
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

	myTextures[t_GBufferNormalDepth]	= { aWidth, aHeight, DXGI_FORMAT_R16G16B16A16_UNORM };
	myTextures[t_GBufferAlbedo]			= { aWidth, aHeight, DXGI_FORMAT_R8G8B8A8_UNORM};
	myTextures[t_GBufferMetalRoughAo]	= { aWidth, aHeight, DXGI_FORMAT_R8G8B8A8_UNORM };
	myTextures[t_GBufferEntity]			= { aWidth, aHeight, DXGI_FORMAT_R32_UINT };
	myTextures[t_AmbientAccessMap]		= { aWidth / 2, aHeight / 2, DXGI_FORMAT_R16_UNORM };
	myTextures[t_BlurInputTexture]		= { aWidth / 2, aHeight / 2, DXGI_FORMAT_R16_UNORM };
	myTextures[t_LightingTexture]		= { aWidth, aHeight, DXGI_FORMAT_R32G32B32A32_FLOAT };

	for (auto& [slot, texture] : myTextures)
	{
		if (!texture)
			return false;
	}

	myWidth = aWidth;
	myHeight = aHeight;

	return true;
}

void Renderer::SetCamera(const Camera& aCamera, const Matrix& aTransform, bool useScreenAspect)
{
	Camera camera{ aCamera };
	if (useScreenAspect)
		camera.SetAspect(static_cast<float>(myWidth) / static_cast<float>(myHeight));

	// Update constant buffer
	{
		CameraBuffer buffer{};
		buffer.viewProj = aTransform.Invert() * camera.GetViewMatrix() * camera.GetProjectionMatrix(USE_REVERSE_Z);
		buffer.invViewProj = buffer.viewProj.Invert();
		buffer.clipPlanes.x = camera.GetNearZ();
		buffer.clipPlanes.y = camera.GetFarZ();
		buffer.position = (XMVECTOR)aTransform.Translation();
		buffer.position.w = 1.f;

		myCBuffers.at(b_Camera).Update(&buffer);
	}

	// Update bounding frustum
	{
		BoundingFrustum::CreateFromMatrix(myFrustum, camera.GetProjectionMatrix() );
		myFrustum.Transform(myFrustum, aTransform);
	}
}

void Renderer::Render(const entt::registry& aRegistry)
{
	if (!operator bool())
		return;

	Clear();

	RenderGeometry(aRegistry);

	ScopedResources scopedGBuffer
	{
		ShaderType::Pixel, t_GBufferNormalDepth,
		{
			myTextures.at(t_GBufferNormalDepth),
			myTextures.at(t_GBufferAlbedo),
			myTextures.at(t_GBufferMetalRoughAo),
			myTextures.at(t_GBufferEntity),
		}
	};

	if (settings.ssao)
		RenderOcclusion();

	ScopedResources scopedAccess{ ShaderType::Pixel, t_AmbientAccessMap, { myTextures.at(t_AmbientAccessMap) } };

	RenderLights(aRegistry);

	ScopedResources scopedLight{ ShaderType::Pixel, t_LightingTexture, { myTextures.at(t_LightingTexture) } };

	switch (settings.output)
	{
	case RenderOutput::Final:
	{
		FullscreenPass{ "PsTonemapAndGamma.cso" }.Render();
		break;
	}
	case RenderOutput::Depth:
	{
		FullscreenPass{ "PsDepth.cso" }.Render();
		break;
	}
	case RenderOutput::Normal:
	{
		FullscreenPass{ "PsNormal.cso" }.Render();
		break;
	}
	case RenderOutput::Position:
	{
		FullscreenPass{ "PsPosition.cso" }.Render();
		break;
	}
	case RenderOutput::Entity:
	{
		FullscreenPass{ "PsEntity.cso" }.Render();
		break;
	}
	case RenderOutput::Access:
	{
		FullscreenPass{ "PsAccess.cso" }.Render();
		break;
	}
	}
}

entt::entity Renderer::PickEntity(unsigned x, unsigned y)
{
	entt::entity entity{ entt::null };
	std::span span{ &entity, 1 };
	myTextures.at(t_GBufferEntity).GetTexel(std::as_writable_bytes(span), x, y);
	return entity;
}

Renderer::operator bool() const
{
	return mySucceeded;
}

void Renderer::Clear()
{
	statistics = {};

	myDepthBuffer.Clear(FAR_Z);

	for (auto& [slot, texture] : myTextures)
	{
		switch (slot)
		{
		case t_GBufferNormalDepth:
		{
			texture.Clear({ 0.f, 0.f, 0.f, FAR_Z });
			break;
		}
		case t_AmbientAccessMap:
		{
			texture.Clear({ 1.f, 1.f, 1.f, 1.f });
			break;
		}
		case t_GBufferEntity:
		{
			ScopedTargets scopedTargets{ texture };
			FullscreenPass{ "PsClearEntity.cso" }.Render();
			break;
		}
		default:
		{
			texture.Clear();
			break;
		}
		}
	}
}

void Renderer::RenderGeometry(const entt::registry& aRegistry)
{
#if USE_REVERSE_Z
	CD3D11_DEPTH_STENCIL_DESC depth{ CD3D11_DEFAULT{} };
	depth.DepthFunc = D3D11_COMPARISON_GREATER;
	ScopedDepthStencil scopedDepth{ depth };
#endif

	ScopedLayout scopedLayout{ typeid(VsInBasic) };
	ScopedShader scopedVs{ "VsBasic.cso" };
	ScopedShader scopedPs{ "PsGBuffer.cso" };
	ScopedTargets scopedTargets
	{
		{
			myTextures.at(t_GBufferNormalDepth),
			myTextures.at(t_GBufferAlbedo),
			myTextures.at(t_GBufferMetalRoughAo),
			myTextures.at(t_GBufferEntity),
		},
		myDepthBuffer
	};

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

		ScopedResources scopedResources
		{
			ShaderType::Pixel, t_MaterialAlbedo,
			{
				material.GetResource(TextureType::Albedo),
				material.GetResource(TextureType::Normal),
				material.GetResource(TextureType::Metallic),
				material.GetResource(TextureType::Roughness),
				material.GetResource(TextureType::Occlusion)
			}
		};

		mesh->Draw();
		statistics.meshes++;
	}
}

void Renderer::RenderOcclusion()
{
	auto& ambientMap{ myTextures.at(t_AmbientAccessMap) };
	auto& blurTexture{ myTextures.at(t_BlurInputTexture) };

	ScopedViewports scopedViewports{ ambientMap.GetViewport() };

	// Compute ambient access map
	{
		ScopedTargets scopedTarget{ ambientMap };
		FullscreenPass{ "PsSSAO.cso" }.Render();
	}
	
	for (size_t i = 0; i < 4; ++i)
	{
		// Blur horizontally
		{
			ScopedResources scopedResource{ ShaderType::Pixel, t_BlurInputTexture, { ambientMap } };
			ScopedTargets scopedTarget{ blurTexture };
			FullscreenPass{ "PsBlurHorizontal.cso" }.Render();
		}

		// Blur vertically
		{
			ScopedResources scopedResource{ ShaderType::Pixel, t_BlurInputTexture, { blurTexture } };
			ScopedTargets scopedTarget{ ambientMap };
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

	ScopedTargets scopedTargets{ myTextures.at(t_LightingTexture) };
	ScopedBlend scopedBlend{ blendDesc };

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
		ScopedResources scopedCubemap{ ShaderType::Pixel, t_EnvironmentMap, cubemap->GetMaps() };
		FullscreenPass{ "PsImageBasedLight.cso" }.Render();

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

	ScopedTopology scopedTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };
	ScopedLayout scopedLayout{ typeid(EmptyVertex) };
	ScopedShader scopedVs{ "VsCubemap.cso" };
	ScopedShader scopedGs{ "GsSkybox.cso" };
	ScopedShader scopedPs{ "PsSkybox.cso" };
	ScopedTargets scopedTargets{ { myTextures.at(t_LightingTexture) }, myDepthBuffer };
	ScopedDepthStencil scopedDepthStencil{ depthStencil };

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

/*
* namespace ImGui
*/

void ImGui::InspectRenderer(Renderer& aRenderer)
{
	if (TreeNode(ICON_FA_GEARS" Settings"))
	{
		RenderSettings& settings{ aRenderer.settings };

		if (BeginCombo("Output", RenderOutputToString(settings.output)))
		{
			for (int i = 0; i < std::to_underlying(RenderOutput::Count); ++i)
			{
				RenderOutput output{ i };
				if (Selectable(RenderOutputToString(output), settings.output == output))
					settings.output = output;
			}
			EndCombo();
		}

		Checkbox("SSAO", &aRenderer.settings.ssao);

		TreePop();
	}

	if (TreeNode(ICON_FA_CHART_SIMPLE" Statistics"))
	{
		const RenderStatistics& stats{ aRenderer.statistics };

		Value("Meshes", stats.meshes);
		Value("Dir. Lights", stats.dLights);
		Value("Point Lights", stats.pLights);
		Value("Spot Lights", stats.sLights);

		TreePop();
	}
}
