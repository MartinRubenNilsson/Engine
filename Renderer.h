#pragma once
#include "DepthBuffer.h"
#include "RenderTargets.h"
#include "FullscreenPass.h"
#include "Cubemap.h"

class Renderer
{
public:
	int pass = 0;

	Renderer() = default;
	Renderer(unsigned aWidth, unsigned aHeight);

	void CreateBuffers(unsigned aWidth, unsigned aHeight);
	void Render(entt::registry&);

	explicit operator bool() const;

private:
	void ClearBuffers();
	void RenderGeometry(entt::registry&);
	void RenderLightning();
	void RenderSkybox();
	void TonemapAndGammaCorrect();

	DepthBuffer myDepthBuffer{};
	RenderTargets myGeometryBuffer{};
	RenderTargets myLightningBuffer{};
	Cubemap mySkybox{};
};

