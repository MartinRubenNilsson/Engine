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
	void Render(entt::registry&);
	void Render(TextureSlot);

	entt::entity PickEntity(unsigned x, unsigned y);

	RenderStatistics GetStatistics() const { return myStatistics; }

	explicit operator bool() const { return mySucceeded; }

private:
	void Clear();
	void RenderGeometry(entt::registry&);
	void RenderSSAO();
	void RenderLightning(entt::registry&);
	void RenderSkybox();
	void RenderDirectionalLights(std::span<const DirectionalLight>);
	void RenderPointLights(std::span<const PointLight>);
	void RenderSpotLights(std::span<const SpotLight>);

	std::vector<RenderTargetPtr> GetGBufferTargets() const;
	std::vector<ShaderResourcePtr> GetGBufferResources() const;

	bool mySucceeded{ false };
	ScopedSamplerStates mySamplers;
	std::array<ConstantBuffer, CBufferCount> myCBuffers{};
	std::array<RenderTexture, t_LightingTexture + 1> myRenderTextures{};
	DepthBuffer myDepthBuffer{};
	ShaderResourcePtr myGaussianMap{};

	Cubemap myCubemap{};

	BoundingFrustum myFrustum{};
	RenderStatistics myStatistics{};
};

namespace ImGui
{
	void InspectRenderStatistics(const RenderStatistics&);
}

