#pragma once
#include "DepthBuffer.h"
#include "RenderTargets.h"
#include "ConstantBuffer.h"
#include "Pixel.h"
#include "Camera.h"
#include "Light.h"
#include "Cubemap.h"

class Renderer

{
public:
	int pass = 0;

	Renderer() = default;
	Renderer(unsigned aWidth, unsigned aHeight);

	bool ResizeBuffers(unsigned aWidth, unsigned aHeight);

	void SetCamera(const Camera& aCamera, const Matrix& aTransform);
	void Render(entt::registry&);

	entt::entity PickEntity(unsigned x, unsigned y);

	explicit operator bool() const { return mySucceeded; }

private:
	void ClearBuffers();
	void RenderGeometry(entt::registry&);
	void RenderLightning(entt::registry&);
	void RenderSkybox();
	void TonemapAndGammaCorrect();

	void RenderDirectionalLights(std::span<const DirectionalLight>);
	void RenderPointLights(std::span<const PointLight>);
	void RenderSpotLights(std::span<const SpotLight>);

	DepthBuffer myDepthBuffer{};
	RenderTargets myGeometryBuffer{};
	RenderTargets myLightningBuffer{};

	ConstantBuffer myCameraBuffer{};
	ConstantBuffer myMeshBuffer{};
	ConstantBuffer myLightBuffer{};

	Pixel myEntityPixel{};
	Cubemap mySkybox{};

	bool mySucceeded{ false };
};

