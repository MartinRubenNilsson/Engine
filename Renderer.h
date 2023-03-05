#pragma once
#include "ShaderCommon.h"
#include "DepthBuffer.h"
#include "RenderTargets.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "Light.h"
#include "Cubemap.h"

struct RenderStatistics
{
	unsigned renderTimeMs{};
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

	bool Resize(unsigned aWidth, unsigned aHeight);
	void SetCamera(const Camera& aCamera, const Matrix& aTransform);
	void Render(entt::registry&);
	void RenderGBufferTexture(size_t anIndex);

	entt::entity PickEntity(unsigned x, unsigned y);

	RenderStatistics GetStatistics() const { return myStatistics; }

	explicit operator bool() const { return mySucceeded; }

private:
	void Clear();

	void RenderGeometry(entt::registry&);
	void RenderLightning(entt::registry&);
	void RenderSkybox();

	void RenderDirectionalLights(std::span<const DirectionalLight>);
	void RenderPointLights(std::span<const PointLight>);
	void RenderSpotLights(std::span<const SpotLight>);

	ScopedSamplerStates mySamplers;
	std::array<ConstantBuffer, CBufferCount> myCBuffers{};

	DepthBuffer myDepthBuffer{};
	RenderTargets myGeometryBuffer{}, myLightningBuffer{};
	Cubemap myCubemap{};

	RenderStatistics myStatistics{};

	bool mySucceeded{ false };
};

namespace ImGui
{
	void InspectRenderStatistics(const RenderStatistics&);
}

