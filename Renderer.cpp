#include "pch.h"
#include "Renderer.h"
#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "Light.h"
#include "FullscreenPass.h"
#include "Pixel.h"

/*
* class Renderer
*/

Renderer::Renderer(unsigned aWidth, unsigned aHeight)
{
	myCBuffers = { sizeof(CameraBuffer), sizeof(MeshBuffer), sizeof(LightBuffer) };
	if (!std::ranges::all_of(myCBuffers, &ConstantBuffer::operator bool))
		return;

	for (unsigned i = 0; i < CBufferCount; ++i)
	{
		myCBuffers[i].VSSetBuffer(i);
		myCBuffers[i].GSSetBuffer(i);
		myCBuffers[i].PSSetBuffer(i);
	}

	if (!Resize(aWidth, aHeight))
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
		//mySkybox = { "cubemap/hdr/Newport_Loft_Ref.hdr" };
		if (!mySkybox)
			return;
	}

	mySucceeded = true;
}

bool Renderer::Resize(unsigned aWidth, unsigned aHeight)
{
	static constexpr std::array geometryFormats
	{
		DXGI_FORMAT_R16_UNORM,			// Depth
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

	return myDepthBuffer && myGeometryBuffer && myLightningBuffer;
}

void Renderer::SetCamera(const Camera& aCamera, const Matrix& aTransform)
{
	CameraBuffer buffer{};
	buffer.viewProj = aTransform.Invert() * aCamera.GetViewMatrix() * aCamera.GetProjectionMatrix();
	buffer.invViewProj = buffer.viewProj.Invert();
	buffer.position = { aTransform.Translation().operator XMVECTOR() };
	aCamera.GetClipPlanes(buffer.clipPlanes.x, buffer.clipPlanes.y);

	myCBuffers.at(CBufferCamera).Update(&buffer);
}

void Renderer::Render(entt::registry& aRegistry)
{
	if (!operator bool())
		return;

	namespace ch = std::chrono;
	const auto now = ch::steady_clock::now();

	Clear();

	std::array<D3D11_SAMPLER_DESC, SamplerCount> samplers{};
	samplers.fill(CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} });
	samplers[PointSampler].Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplers[TrilinearSampler].Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	ScopedSamplerStates scopedSamplers{ 0, samplers };

	RenderGeometry(aRegistry);
	RenderLightning(aRegistry);
	RenderSkybox();
	TonemapAndGamma();

	const auto duration = ch::duration_cast<ch::milliseconds>(ch::steady_clock::now() - now);
	myStatistics.renderTimeMs = static_cast<unsigned>(duration.count());
}

void Renderer::RenderGBufferTexture(size_t anIndex)
{
	std::array<FullscreenPass, 6> passes
	{
		PIXEL_SHADER("PsGBufferDepth.cso"),
		PIXEL_SHADER("PsGBufferVertexNormal.cso"),
		PIXEL_SHADER("PsGBufferPixelNormal.cso"),
		PIXEL_SHADER("PsGBufferAlbedo.cso"),
		PIXEL_SHADER("PsGBufferMetalRoughAo.cso"),
		PIXEL_SHADER("PsGBufferEntity.cso"),
	};

	if (anIndex < passes.size())
	{
		ScopedShaderResources scopedResources{ ShaderType::Pixel, 0, myGeometryBuffer };
		passes.at(anIndex).Render();
	}
}

entt::entity Renderer::PickEntity(unsigned x, unsigned y)
{
	entt::entity entity{ entt::null };

	Pixel pixel{ DXGI_FORMAT_R32_UINT };
	if (pixel)
	{
		pixel.Copy(myGeometryBuffer.GetTexture(TEXTURE_SLOT_GBUFFER_ENTITY), x, y);
		pixel.Read(&entity, sizeof(entity));
	}

	return entity;
}

void Renderer::Clear()
{
	myStatistics = {};

	myDepthBuffer.Clear();
	myGeometryBuffer.Clear();
	myLightningBuffer.Clear();

	{
		ScopedRenderTargets scopedTargets{ myGeometryBuffer.GetRenderTarget(TEXTURE_SLOT_GBUFFER_DEPTH) };
		FullscreenPass{ PIXEL_SHADER("PsClearDepth.cso") }.Render();
	}
	{
		ScopedRenderTargets scopedTargets{ myGeometryBuffer.GetRenderTarget(TEXTURE_SLOT_GBUFFER_ENTITY) };
		FullscreenPass{ PIXEL_SHADER("PsClearEntity.cso") }.Render();
	}
}

void Renderer::RenderGeometry(entt::registry& aRegistry)
{
	if (aRegistry.empty())
		return;

	ScopedInputLayout scopedLayout{ typeid(VsInBasic) };
	ScopedShader scopedVs{ VERTEX_SHADER("VsBasic.cso") };
	ScopedShader scopedPs{ PIXEL_SHADER("PsGBuffer.cso") };
	ScopedRenderTargets scopedTargets{ myGeometryBuffer, myDepthBuffer };

	auto view = aRegistry.view<const Material::Ptr, const Mesh::Ptr, const Transform::Ptr>();
	for (auto [entity, material, mesh, transform] : view.each())
	{
		mesh->SetVertexAndIndexBuffers();

		ScopedShaderResources scopedResources{ ShaderType::Pixel, 10, *material }; // TODO: turn hardcoded 10 into am acro
		
		MeshBuffer buffer{};
		buffer.matrix = transform->GetWorldMatrix();
		buffer.matrixInvTrans = buffer.matrix.Invert().Transpose();
		std::ranges::fill(buffer.entity, std::to_underlying(entity));

		myCBuffers.at(CBufferMesh).Update(&buffer);

		DX11_CONTEXT->DrawIndexed(mesh->GetIndexCount(), 0, 0);
		myStatistics.meshDrawCalls++;
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

	ScopedShaderResources scopedResources{ ShaderType::Pixel, TEXTURE_SLOT_GBUFFER, myGeometryBuffer };
	ScopedRenderTargets scopedTargets{ myLightningBuffer };
	ScopedBlendState scopedBlend{ blendDesc };

	RenderDirectionalLights(dLights);
	RenderPointLights(pLights);
	RenderSpotLights(sLights);
}

void Renderer::RenderSkybox()
{
	ScopedRenderTargets scopedTargets{ myLightningBuffer, myDepthBuffer };
	mySkybox.DrawSkybox();
}

void Renderer::TonemapAndGamma()
{
	ScopedShaderResources scopedResources{ ShaderType::Pixel, 0, myLightningBuffer };
	FullscreenPass pass{ PIXEL_SHADER("PsTonemapAndGamma.cso") };
	pass.Render();
}

void Renderer::RenderDirectionalLights(std::span<const DirectionalLight> someLights)
{
	FullscreenPass pass{ PIXEL_SHADER("PsDirectionalLight.cso") };

	for (const DirectionalLight& light : someLights)
	{
		LightBuffer buffer{};
		buffer.color = light.color;
		buffer.direction = { light.direction.x, light.direction.y, light.direction.z, 0.f };

		myCBuffers.at(CBufferLight).Update(&buffer);

		pass.Render();
		myStatistics.dirLightDrawCalls++;
	}
}

void Renderer::RenderPointLights(std::span<const PointLight> someLights)
{
	FullscreenPass pass{ PIXEL_SHADER("PsPointLight.cso") };

	for (const PointLight& light : someLights)
	{
		LightBuffer buffer{};
		buffer.color = light.color;
		buffer.position = { light.position.x, light.position.y, light.position.z, 1.f };
		buffer.parameters = light.parameters;

		myCBuffers.at(CBufferLight).Update(&buffer);

		pass.Render();
		myStatistics.pointLightDrawCalls++;
	}
}

void Renderer::RenderSpotLights(std::span<const SpotLight> someLights)
{
	FullscreenPass pass{ PIXEL_SHADER("PsSpotLight.cso") };

	for (const SpotLight& light : someLights)
	{
		LightBuffer buffer{};
		buffer.color = light.color;
		buffer.position = { light.position.x, light.position.y, light.position.z, 1.f };
		buffer.direction = { light.direction.x, light.direction.y, light.direction.z, 0.f };
		buffer.parameters = light.parameters;
		buffer.coneAngles = { light.innerAngle, light.outerAngle, 0.f, 0.f };

		myCBuffers.at(CBufferLight).Update(&buffer);

		pass.Render();
		myStatistics.spotLightDrawCalls++;
	}
}

/*
* namespace ImGui
*/

void ImGui::InspectRenderStatistics(const RenderStatistics& someStats)
{
	Value("Render Time (ms)", someStats.renderTimeMs);

	if (TreeNodeEx("Draw Calls", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Value("Meshes", someStats.meshDrawCalls);
		Value("Dir. Lights", someStats.dirLightDrawCalls);
		Value("Point Lights", someStats.pointLightDrawCalls);
		Value("Spot Lights", someStats.spotLightDrawCalls);
		TreePop();
	}
}
