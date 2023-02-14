#pragma once
#include "BackBuffer.h"
#include "DepthBuffer.h"
#include "RenderTargets.h"
#include "FullscreenPass.h"
#include "Cubemap.h"

class Renderer
{
public:
	Renderer() = default;
	Renderer(HWND);

	void ClearBuffers();
	void RenderGeometry(entt::registry&);
	void RenderLightning();
	void RenderSkybox();

	auto& GetBackBuffer() const { return myBackBuffer; }

	explicit operator bool() const { return mySucceeded; }

private:
	bool mySucceeded{ false };
	BackBuffer myBackBuffer{};
	DepthBuffer myZBuffer{};
	RenderTargets myGBuffer{};
	std::array<FullscreenPass, 6> myFullscreenPasses;
	Cubemap myCubemap{};
};

