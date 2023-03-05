#include "pch.h"
#include "FullscreenPass.h"

FullscreenPass::FullscreenPass(const fs::path& aPixelShader)
	: myPixelShader{ aPixelShader }
{
}

void FullscreenPass::Render() const
{
	ScopedInputLayout scopedLayout{ typeid(EmptyVertex) };
	ScopedShader scopedVs{ "VsFullscreenTriangle.cso" };
	ScopedShader scopedPs{ myPixelShader };

	DX11_CONTEXT->Draw(3, 0);
}

