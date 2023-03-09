#include "pch.h"
#include "FullscreenPass.h"
#include "ShaderCommon.h"

FullscreenPass::FullscreenPass(const fs::path& aPixelShader)
	: myPixelShader{ aPixelShader }
{
}

void FullscreenPass::Render() const
{
	ScopedInputLayout scopedLayout{ typeid(EmptyVertex) };
	ScopedShader scopedVs{ "VsFullscreen.cso" };
	ScopedShader scopedPs{ myPixelShader };

	DX11_CONTEXT->Draw(FULLSCREEN_VERTEX_COUNT, 0);
}

