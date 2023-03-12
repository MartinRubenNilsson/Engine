#pragma once
#include "ShaderCommon.h"
#include "ConstantBuffer.h"
#include "RenderTexture.h"
#include "DepthBuffer.h"
#include "Camera.h"
#include "Light.h"
#include "Cubemap.h"

struct RenderStatistics
{
	unsigned meshDrawCalls{};
	unsigned dirLightDrawCalls{};
	unsigned pointLightDrawCalls{};
	unsigned spotLightDrawCalls{};
};

class Renderer
{
public:
	Renderer() = default;
	Renderer(unsigned aWidth, unsigned aHeight);

	bool ResizeTextures(unsigned aWidth, unsigned aHeight);
	void SetCamera(const Camera& aCamera, const Matrix& aTransform);
	void RenderScene(const entt::registry&);
	void RenderDebug(TextureSlot);

	entt::entity PickEntity(unsigned x, unsigned y);

	RenderStatistics GetStatistics() const { return myStatistics; }

	explicit operator bool() const { return mySucceeded; }

private:
	void Clear();

	void RenderGeometry(const entt::registry&);
	void RenderSSAO();
	void RenderLightning(const entt::registry&);
	void RenderSkybox();

	void RenderImageBasedLight();
	void RenderDirectionalLights(std::span<const DirectionalLight>);
	void RenderPointLights(std::span<const PointLight>);
	void RenderSpotLights(std::span<const SpotLight>);

	std::vector<RenderTargetPtr> GetGBufferTargets() const;
	std::vector<ShaderResourcePtr> GetGBufferResources() const;

	bool mySucceeded{ false };
	ScopedPrimitiveTopology myTopology;
	ScopedSamplerStates mySamplers;
	ShaderResourcePtr myGaussianMap{}, myIntegrationMap{}; // Precomputed maps
	std::array<ConstantBuffer, CBufferCount> myCBuffers{};
	std::array<RenderTexture, t_LightingTexture + 1> myRenderTextures{};
	DepthBuffer myDepthBuffer{};

	Cubemap myCubemap{};

	BoundingFrustum myFrustum{};
	RenderStatistics myStatistics{};
};

namespace ImGui
{
	void InspectRenderStatistics(const RenderStatistics&);
}

