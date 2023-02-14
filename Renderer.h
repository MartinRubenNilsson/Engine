#pragma once
#include "BackBuffer.h"
#include "DepthBuffer.h"
#include "RenderTargets.h"
#include "FullscreenPass.h"
#include "Cubemap.h"

class Renderer
{
public:
	int pass = 0;

	Renderer() = default;
	Renderer(HWND);

	void Render(entt::registry&);

	auto& GetBackBuffer() const { return myBackBuffer; }

	explicit operator bool() const { return mySucceeded; }

private:
	void ClearBuffers();
	void RenderGeometry(entt::registry&);
	void RenderLightning();
	void RenderSkybox();

	bool mySucceeded{ false };
	BackBuffer myBackBuffer{};
	DepthBuffer myDepthBuffer{};
	RenderTargets myGeometryBuffer{};
	std::array<FullscreenPass, 7> myFullscreenPasses;
	Cubemap mySkybox{};
};

