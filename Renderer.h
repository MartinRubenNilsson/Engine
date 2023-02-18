#pragma once
#include "DepthBuffer.h"
#include "RenderTargets.h"
#include "ConstantBuffer.h"
#include "Pixel.h"
#include "Cubemap.h"
#include "Light.h"

class Renderer
{
public:
	int pass = 0;

	Renderer() = default;
	Renderer(unsigned aWidth, unsigned aHeight);

	void ResizeBuffers(unsigned aWidth, unsigned aHeight);
	void Render(entt::registry&);

	entt::entity PickEntity(unsigned x, unsigned y);

	explicit operator bool() const;

private:
	void ClearBuffers();
	void RenderGeometry(entt::registry&);
	void RenderLightning(entt::registry&);
	void RenderSkybox();
	void TonemapAndGammaCorrect();

	void RenderDirectionalLights(std::span<const DirectionalLight>);
	void RenderPointLights(std::span<const PointLight>);

	DepthBuffer myDepthBuffer{};
	RenderTargets myGeometryBuffer{};
	RenderTargets myLightningBuffer{};

	ConstantBuffer myMeshBuffer{};
	ConstantBuffer myLightBuffer{};

	Pixel myEntityPixel{};
	Cubemap mySkybox{};
};

