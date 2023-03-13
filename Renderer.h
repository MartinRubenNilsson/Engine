#pragma once
#include "ShaderCommon.h"
#include "ConstantBuffer.h"
#include "RenderTexture.h"
#include "DepthBuffer.h"
#include "Camera.h"

enum class RenderOutput
{
	Final,
	Depth,
	Normal,
	Position,
	Entity,
	Access,
	Count
};

const char* RenderOutputToString(RenderOutput);

struct RenderSettings
{
	bool ssao{ true };
};

struct RenderStatistics
{
	unsigned meshes{};
	unsigned dLights{}; // Directional
	unsigned pLights{}; // Point
	unsigned sLights{}; // Spot
};

class Renderer
{
public:
	RenderOutput output{};
	RenderSettings settings{};
	RenderStatistics statistics{};

	Renderer() = default;
	Renderer(unsigned aWidth, unsigned aHeight);

	bool ResizeTextures(unsigned aWidth, unsigned aHeight);
	void SetCamera(const Camera& aCamera, const Matrix& aTransform);
	void Render(const entt::registry&);

	entt::entity PickEntity(unsigned x, unsigned y);

	explicit operator bool() const { return mySucceeded; }

private:
	void Clear();

	void RenderGeometry(const entt::registry&);
	void RenderSSAO();
	void RenderLights(const entt::registry&);
	void RenderSkybox();

	void RenderDirectionalLights(std::span<const LightBuffer>);
	void RenderPointLights(std::span<const LightBuffer>);
	void RenderSpotLights(std::span<const LightBuffer>);

	std::vector<RenderTargetPtr> GetGBufferTargets() const;
	std::vector<ShaderResourcePtr> GetGBufferResources() const;

	bool mySucceeded{ false };
	ScopedPrimitiveTopology myTopology;
	ScopedSamplerStates mySamplers;
	ShaderResourcePtr myGaussianMap{}, myIntegrationMap{}; // Precomputed maps
	std::array<ConstantBuffer, CBufferCount> myCBuffers{};
	std::array<RenderTexture, t_LightingTexture + 1> myRenderTextures{};
	DepthBuffer myDepthBuffer{};
	BoundingFrustum myFrustum{};
};

namespace ImGui
{
	void InspectRenderer(Renderer&);
}

