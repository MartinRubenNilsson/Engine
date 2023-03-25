#pragma once
#include "ShaderCommon.h"
#include "ConstantBuffer.h"
#include "RenderTexture.h"
#include "DepthBuffer.h"

class Camera;

enum class RenderOutput
{
	Final,
	Depth,
	Normal,
	Position,
	Entity,
	Occlusion,
	Count
};

enum class OcclusionType
{
	None,
	SSAO,
	HBAO,
	Count,
};

const char* ToString(RenderOutput);
const char* ToString(OcclusionType);

struct RenderSettings
{
	RenderOutput output = RenderOutput::Final;
	OcclusionType occlusion = OcclusionType::SSAO;
};

struct RenderStats
{
	unsigned meshes{};
	unsigned dirLights{};
	unsigned pointLights{};
	unsigned spotLights{};
};

class Renderer : public Singleton<Renderer>
{
public:
	RenderSettings settings{};
	RenderStats stats{};

	Renderer() = default;
	Renderer(unsigned aWidth, unsigned aHeight);

	bool ResizeTextures(unsigned aWidth, unsigned aHeight);
	void SetCamera(const Camera& aCamera, const Matrix& aTransform);
	void Render(const entt::registry&);

	RenderTexture& GetTexture(TextureSlot);

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
	void Inspect(Renderer&);
}

