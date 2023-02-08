#include "pch.h"
#include "FullscreenPass.h"

FullscreenPass::FullscreenPass(std::shared_ptr<const PixelShader> aPixelShader)
	: myPixelShader{ aPixelShader }
{
}

void FullscreenPass::Draw() const
{
	if (!operator bool())
		return;

	ScopedInputLayout layout{ typeid(EmptyVertex) };
	ScopedShader vs{ VERTEX_SHADER("VsFullscreen.cso") };
	ScopedShader ps{ myPixelShader };

	DX11_CONTEXT->Draw(3, 0);
}
