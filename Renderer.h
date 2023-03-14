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
	RenderOutput output{};
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
	RenderSettings settings{};
	RenderStatistics statistics{};

	Renderer() = default;
	Renderer(unsigned aWidth, unsigned aHeight);

	bool ResizeTextures(unsigned aWidth, unsigned aHeight);
	void SetCamera(const Camera& aCamera, const Matrix& aTransform, bool useScreenAspect = true);
	void Render(const entt::registry&);

	entt::entity PickEntity(unsigned x, unsigned y);

	explicit operator bool() const;

private:
	void Clear();

	void RenderGeometry(const entt::registry&);
	void RenderOcclusion();
	void RenderLights(const entt::registry&);
	void RenderSkybox();

	void RenderDirectionalLights(std::span<const LightBuffer>);
	void RenderPointLights(std::span<const LightBuffer>);
	void RenderSpotLights(std::span<const LightBuffer>);

	std::unordered_map<CBufferSlot, ConstantBuffer> myCBuffers{};
	std::unordered_map<TextureSlot, RenderTexture> myTextures{};
	DepthBuffer myDepthBuffer{};
	BoundingFrustum myFrustum{};
	unsigned myWidth{}, myHeight{};
	bool mySucceeded{ false };
};

namespace ImGui
{
	void InspectRenderer(Renderer&);
}

