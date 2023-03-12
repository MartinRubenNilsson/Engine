#include "pch.h"
#include "FullscreenPass.h"
#include "ShaderCommon.h"

FullscreenPass::FullscreenPass(const fs::path& aPixelShader)
	: myLayout{ typeid(EmptyVertex) }
	, myVertexShader{ "VsFullscreen.cso" }
	, myPixelShader{ aPixelShader }
{
}

void FullscreenPass::Render() const
{
	DX11_CONTEXT->Draw(FULLSCREEN_VERTEX_COUNT, 0);
}

