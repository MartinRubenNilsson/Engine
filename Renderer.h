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

	void Render(entt::registry&);

	explicit operator bool() const { return mySucceeded; }

private:
	void ClearBuffers();
	void RenderGeometry(entt::registry&);
	void RenderLightning();
	void RenderSkybox();
	void TonemapAndGammaCorrect();

	bool mySucceeded{ false };
	unsigned myWidth{}, myHeight{};

	RenderTargets myGeometryBuffer{};
	RenderTargets myOutputBuffer{};
	DepthBuffer myDepthBuffer{};
	std::array<FullscreenPass, 7> myFullscreenPasses;
	Cubemap mySkybox{};
};

