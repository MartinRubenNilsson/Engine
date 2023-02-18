#include "pch.h"
#include "FullscreenPass.h"

FullscreenPass::FullscreenPass(std::shared_ptr<const PixelShader> aPixelShader)
	: myPixelShader{ aPixelShader }
{
}

void FullscreenPass::Render() const
{
	if (!operator bool())
		return;

	ScopedInputLayout scopedLayout{ typeid(EmptyVertex) };
	ScopedShader scopedVs{ VERTEX_SHADER("VsFullscreenPass.cso") };
	ScopedShader scopedPs{ myPixelShader };

	DX11_CONTEXT->Draw(3, 0);
}

FullscreenPass::operator bool() const
{
	return myPixelShader.operator bool();
}
