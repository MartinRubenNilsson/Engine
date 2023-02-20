#pragma once
#include "ShaderCommon.h"
#include "DepthBuffer.h"
#include "RenderTargets.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include "Light.h"
#include "Cubemap.h"

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

	explicit operator bool() const { return mySucceeded; }

private:
	void Clear();
	void RenderGeometry(entt::registry&);
	void RenderLightning(entt::registry&);
	void RenderSkybox();
	void TonemapAndGammaCorrect();

	void RenderDirectionalLights(std::span<const DirectionalLight>);
	void RenderPointLights(std::span<const PointLight>);
	void RenderSpotLights(std::span<const SpotLight>);

	std::array<ConstantBuffer, CBufferCount> myCBuffers{};
	DepthBuffer myDepthBuffer{};
	RenderTargets myGeometryBuffer{}, myLightningBuffer{};
	Cubemap mySkybox{};

	bool mySucceeded{ false };
};

