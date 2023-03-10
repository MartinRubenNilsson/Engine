#include "pch.h"
#include "Renderer.h"
#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "Light.h"
#include "FullscreenPass.h"

namespace
{
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
	: mySamplers{ 0, GetSamplerDescs() }
	, myCBuffers{ sizeof(ImmutableBuffer), sizeof(CameraBuffer), sizeof(MeshBuffer), sizeof(LightBuffer) }
	, myGaussianMap{ CreateGaussianMap() }
{
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

	if (!myGaussianMap)
		return;

	if (!ResizeTextures(aWidth, aHeight))
		return;

	myCubemap = { "cubemap/hdr/Newport_Loft_Ref.hdr" };
	if (!myCubemap)
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
	const Matrix view{ aTransform.Invert() * aCamera.GetViewMatrix() };
	const Matrix proj{ aCamera.GetProjectionMatrix(USE_REVERSE_Z) };

	// Update constant buffer
	{
		CameraBuffer buffer{};
		buffer.viewProj = view * proj;
		buffer.invViewProj = buffer.viewProj.Invert();
		buffer.position = { aTransform._41, aTransform._42, aTransform._43, 1.f };
		buffer.clipPlanes = { aCamera.GetNearZ(), aCamera.GetFarZ(), 0.f, 0.f };

		myCBuffers.at(b_Camera).Update(&buffer);
	}

	// Update bounding frustum
	{
		BoundingFrustum::CreateFromMatrix(myFrustum, aCamera.GetProjectionMatrix() );
		myFrustum.Transform(myFrustum, aTransform);
	}
}

void Renderer::Render(entt::registry& aRegistry)
{
	if (!operator bool())
		return;

	Clear();
	RenderGeometry(aRegistry);
	{
		ScopedShaderResources scopedGBuffer{ ShaderType::Pixel, GBUFFER_BEGIN, GetGBufferResources() };
		RenderSSAO();

		ScopedShaderResources scopedCubemap{ ShaderType::Pixel, t_EnvironmentMap, myCubemap.GetMaps() };
		RenderLightning(aRegistry);
		RenderSkybox();
	}
	{
		ScopedShaderResources scopedLightning{ ShaderType::Pixel, t_LightingTexture, myRenderTextures.at(t_LightingTexture) };
		FullscreenPass{ "PsTonemapAndGamma.cso" }.Render();
	}
}

void Renderer::Render(TextureSlot aSlot)
{
	switch (aSlot)
	{
	case t_Normal:
	{
		ScopedShaderResources scopedResource{ ShaderType::Pixel, t_GBufferNormalDepth, myRenderTextures.at(t_GBufferNormalDepth) };
		FullscreenPass{ "PsGetNormal.cso" }.Render();
		break;
	}
	case t_Depth:
	{
		ScopedShaderResources scopedResource{ ShaderType::Pixel, t_GBufferNormalDepth, myRenderTextures.at(t_GBufferNormalDepth) };
		FullscreenPass{ "PsGetDepth.cso" }.Render();
		break;
	}
	case t_Position:
	{
		ScopedShaderResources scopedResource{ ShaderType::Pixel, t_GBufferNormalDepth, myRenderTextures.at(t_GBufferNormalDepth) };
		FullscreenPass{ "PsGetPosition.cso" }.Render();
		break;
	}
	case t_GBufferEntity:
	{
		ScopedShaderResources scopedResource{ ShaderType::Pixel, aSlot, myRenderTextures.at(aSlot) };
		FullscreenPass{ "PsGetEntity.cso" }.Render();
		break;
	}
	case t_GaussianMap:
	{
		ScopedShaderResources scopedResource{ ShaderType::Pixel, t_GaussianMap, myGaussianMap };
		FullscreenPass{ "PsGetGaussian.cso" }.Render();
		break;
	}
	default:
	{
		if (aSlot < myRenderTextures.size())
		{
			ScopedShaderResources scopedResource{ ShaderType::Pixel, 0, myRenderTextures.at(aSlot) };
			FullscreenPass{ "PsPointSample.cso" }.Render();
		}
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
	myDepthBuffer.Clear(FAR_Z);

	for (RenderTexture& texture : myRenderTextures)
		texture.Clear();

	{
		ScopedRenderTargets scopedTargets{ myRenderTextures.at(t_GBufferEntity) };
		FullscreenPass{ "PsClearEntity.cso" }.Render();
	}

	myStatistics = {};
}

void Renderer::RenderGeometry(entt::registry& aRegistry)
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
		myStatistics.meshDrawCalls++;
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

void Renderer::RenderLightning(entt::registry& aRegistry)
{
	std::vector<DirectionalLight> dLights{};
	std::vector<PointLight> pLights{};
	std::vector<SpotLight> sLights{};

	auto view = aRegistry.view<const Light, const Transform::Ptr>();
	for (auto [entity, light, transform] : view.each())
	{
		if (!light.enabled)
			continue;

		const Matrix worldMatrix{ transform->GetWorldMatrix() }; // todo: remove scale

		// todo: culling

		switch (light.GetType())
		{
		case LightType::Directional:
		{
			auto& dLight{ dLights.emplace_back(light.Get<DirectionalLight>()) };
			dLight.color.Premultiply();
			Vector3::TransformNormal(dLight.direction, worldMatrix).Normalize(dLight.direction);
			break;
		}
		case LightType::Point:
		{
			auto& pLight{ pLights.emplace_back(light.Get<PointLight>()) };
			pLight.color.Premultiply();
			Vector3::Transform(pLight.position, worldMatrix, pLight.position);
			break;
		}
		case LightType::Spot:
		{
			auto& sLight{ sLights.emplace_back(light.Get<SpotLight>()) };
			sLight.color.Premultiply();
			Vector3::Transform(sLight.position, worldMatrix, sLight.position);
			Vector3::TransformNormal(sLight.direction, worldMatrix).Normalize(sLight.direction);
			break;
		}
		}
	}

	CD3D11_BLEND_DESC blendDesc{ CD3D11_DEFAULT{} };
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	ScopedRenderTargets scopedTargets{ myRenderTextures.at(t_LightingTexture) };
	ScopedBlendState scopedBlend{ blendDesc };

	RenderImageBasedLight();
	RenderDirectionalLights(dLights);
	RenderPointLights(pLights);
	RenderSpotLights(sLights);
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

	DX11_CONTEXT->Draw(14, 0);
}

void Renderer::RenderImageBasedLight()
{
	ScopedShaderResources scopedResource{ ShaderType::Pixel, t_AmbientAccessMap, myRenderTextures.at(t_AmbientAccessMap) };
	FullscreenPass{ "PsImageBasedLight.cso" }.Render();
}

void Renderer::RenderDirectionalLights(std::span<const DirectionalLight> someLights)
{
	FullscreenPass pass{ "PsDirectionalLight.cso" };

	for (const DirectionalLight& light : someLights)
	{
		LightBuffer buffer{};
		buffer.color = light.color;
		buffer.direction = { light.direction.x, light.direction.y, light.direction.z, 0.f };

		myCBuffers.at(b_Light).Update(&buffer);

		pass.Render();
		myStatistics.dirLightDrawCalls++;
	}
}

void Renderer::RenderPointLights(std::span<const PointLight> someLights)
{
	FullscreenPass pass{ "PsPointLight.cso" };

	for (const PointLight& light : someLights)
	{
		LightBuffer buffer{};
		buffer.color = light.color;
		buffer.position = { light.position.x, light.position.y, light.position.z, 1.f };
		buffer.parameters = light.parameters;

		myCBuffers.at(b_Light).Update(&buffer);

		pass.Render();
		myStatistics.pointLightDrawCalls++;
	}
}

void Renderer::RenderSpotLights(std::span<const SpotLight> someLights)
{
	FullscreenPass pass{ "PsSpotLight.cso" };

	for (const SpotLight& light : someLights)
	{
		LightBuffer buffer{};
		buffer.color = light.color;
		buffer.position = { light.position.x, light.position.y, light.position.z, 1.f };
		buffer.direction = { light.direction.x, light.direction.y, light.direction.z, 0.f };
		buffer.parameters = light.parameters;
		buffer.coneAngles = { light.innerAngle, light.outerAngle, 0.f, 0.f };

		myCBuffers.at(b_Light).Update(&buffer);

		pass.Render();
		myStatistics.spotLightDrawCalls++;
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

void ImGui::InspectRenderStatistics(const RenderStatistics& someStats)
{
	if (TreeNodeEx("Draw Calls", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Value("Meshes", someStats.meshDrawCalls);
		Value("Dir. Lights", someStats.dirLightDrawCalls);
		Value("Point Lights", someStats.pointLightDrawCalls);
		Value("Spot Lights", someStats.spotLightDrawCalls);
		TreePop();
	}
}
