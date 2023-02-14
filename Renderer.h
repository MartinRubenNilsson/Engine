#pragma once
#include "RenderTargets.h"
#include "DepthBuffer.h"
#include "FullscreenPass.h"
#include "Cubemap.h"

class Renderer
{
public:
	int pass = 0;

	Renderer() = default;
	Renderer(unsigned aWidth, unsigned aHeight);

	void Render(entt::registry&);

	explicit operator bool() const { return mySucceeded; }

private:
	bool CreateBuffers(unsigned aWidth, unsigned aHeight);

	void ClearBuffers();
	void RenderGeometry(entt::registry&);
	void RenderLightning();
	void RenderSkybox();
	void TonemapAndGammaCorrect();

	bool mySucceeded{ false };
	DepthBuffer myDepthBuffer{};
	RenderTargets myGeometryBuffer{};
	RenderTargets myLightningBuffer{};
	Cubemap mySkybox{};
};

