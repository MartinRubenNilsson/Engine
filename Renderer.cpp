#include "pch.h"
#include "Renderer.h"
#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "Light.h"
#include "FullscreenPass.h"
#include "Pixel.h"

namespace
{
	auto GetSamplerDescs()
	{
		std::array<D3D11_SAMPLER_DESC, 2> samplers{};
		samplers.fill(CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} });
		samplers[s_PointSampler].Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplers[s_TrilinearSampler].Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		return samplers;
	}

	const CubemapBuffer& GetCubemapBuffer()
	{
		static const XMVECTOR o{ 0.f, 0.f, 0.f, 1.f };
		static const XMVECTOR x{ 1.f, 0.f, 0.f, 0.f };
		static const XMVECTOR y{ 0.f, 1.f, 0.f, 0.f };
		static const XMVECTOR z{ 0.f, 0.f, 1.f, 0.f };
		static const XMMATRIX proj{ XMMatrixPerspectiveFovLH(XM_PIDIV2, 1.f, 0.1f, 10.f) };

		static CubemapBuffer buffer
		{
			XMMatrixLookToLH(o, +x, +y) * proj, // +X
			XMMatrixLookToLH(o, -x, +y) * proj, // -X
			XMMatrixLookToLH(o, +y, -z) * proj, // +Y
			XMMatrixLookToLH(o, -y, +z) * proj, // -Y
			XMMatrixLookToLH(o, +z, +y) * proj, // +Z
			XMMatrixLookToLH(o, -z, +y) * proj, // -Z
		};

		return buffer;
	}
}

/*
* class Renderer
*/

Renderer::Renderer(unsigned aWidth, unsigned aHeight)
	: mySamplers{ 0, GetSamplerDescs() }
	, myCBuffers{ sizeof(CameraBuffer), sizeof(MeshBuffer), sizeof(LightBuffer), sizeof(CubemapBuffer) }
{
	if (!std::ranges::all_of(myCBuffers, &ConstantBuffer::operator bool))
		return;

	myCBuffers.at(b_Cubemap).Update(&GetCubemapBuffer());

	for (unsigned i = 0; i < myCBuffers.size(); ++i)
	{
		myCBuffers[i].VSSetBuffer(i);
		myCBuffers[i].GSSetBuffer(i);
		myCBuffers[i].PSSetBuffer(i);
	}

	if (!Resize(aWidth, aHeight))
		return;

	myCubemap = { "cubemap/hdr/Newport_Loft_Ref.hdr" };
	if (!myCubemap)
		return;

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
	buffer.position = { aTransform._41, aTransform._42, aTransform._43, 1.0 };

	myCBuffers.at(b_Camera).Update(&buffer);
}

void Renderer::Render(entt::registry& aRegistry)
{
	if (!operator bool())
		return;

	namespace ch = std::chrono;
	const auto now = ch::steady_clock::now();

	Clear();
	RenderGeometry(aRegistry);
	{
		ScopedShaderResources scopedGBuffer{ ShaderType::Pixel, t_GBufferDepth, myGeometryBuffer };
		ScopedShaderResources scopedCubemap{ ShaderType::Pixel, t_EnvironmentMap, myCubemap.GetMaps() };
		RenderLightning(aRegistry);
		RenderSkybox();
	}
	{
		ScopedShaderResources scopedLightning{ ShaderType::Pixel, 0, myLightningBuffer };
		FullscreenPass{ "PsTonemapAndGamma.cso" }.Render();
	}

	const auto duration = ch::duration_cast<ch::milliseconds>(ch::steady_clock::now() - now);
	myStatistics.renderTimeMs = static_cast<unsigned>(duration.count());
}

void Renderer::RenderGBufferTexture(size_t anIndex)
{
	std::array<FullscreenPass, 7> passes
	{
		fs::path{ "PsGBufferDepth.cso" },
		fs::path{ "PsGBufferWorldPosition.cso" },
		fs::path{ "PsGBufferVertexNormal.cso" },
		fs::path{ "PsGBufferPixelNormal.cso" },
		fs::path{ "PsGBufferAlbedo.cso" },
		fs::path{ "PsGBufferMetalRoughAo.cso" },
		fs::path{ "PsGBufferEntity.cso" },
	};

	ScopedShaderResources scopedResources{ ShaderType::Pixel, 0, myGeometryBuffer };
	passes.at(anIndex).Render();
}

entt::entity Renderer::PickEntity(unsigned x, unsigned y)
{
	entt::entity entity{ entt::null };

	Pixel pixel{ DXGI_FORMAT_R32_UINT };
	if (pixel)
	{
		pixel.Copy(myGeometryBuffer.GetTexture(t_GBufferEntity), x, y);
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
		ScopedRenderTargets scopedTargets{ myGeometryBuffer.GetRenderTarget(t_GBufferDepth) };
		FullscreenPass{ "PsClearDepth.cso" }.Render();
	}
	{
		ScopedRenderTargets scopedTargets{ myGeometryBuffer.GetRenderTarget(t_GBufferEntity) };
		FullscreenPass{ "PsClearEntity.cso" }.Render();
	}
}

void Renderer::RenderGeometry(entt::registry& aRegistry)
{
	if (aRegistry.empty())
		return;

	ScopedInputLayout scopedLayout{ typeid(VsInBasic) };
	ScopedShader scopedVs{ "VsBasic.cso" };
	ScopedShader scopedPs{ "PsGBuffer.cso" };
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

		myCBuffers.at(b_Mesh).Update(&buffer);

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

	ScopedRenderTargets scopedTargets{ myLightningBuffer };
	ScopedBlendState scopedBlend{ blendDesc };

	FullscreenPass{ "PsImageBasedLight.cso" }.Render();
	RenderDirectionalLights(dLights);
	RenderPointLights(pLights);
	RenderSpotLights(sLights);
}

void Renderer::RenderSkybox()
{
	CD3D11_DEPTH_STENCIL_DESC depthStencil{ CD3D11_DEFAULT{} };
	depthStencil.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Otherwise skybox will fail the depth test since z=1.

	ScopedPrimitiveTopology scopedTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };
	ScopedInputLayout scopedLayout{ typeid(EmptyVertex) };
	ScopedShader scopedVs{ "VsCubemap.cso" };
	ScopedShader scopedGs{ "GsSkybox.cso" };
	ScopedShader scopedPs{ "PsSkybox.cso" };
	ScopedRenderTargets scopedTargets{ myLightningBuffer, myDepthBuffer };
	ScopedDepthStencilState scopedDepthStencil{ depthStencil };

	DX11_CONTEXT->Draw(14, 0);
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
