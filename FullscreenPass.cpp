#include "pch.h"
#include "FullscreenPass.h"
#include "InputLayoutManager.h"

FullscreenPass::FullscreenPass(std::shared_ptr<const PixelShader> aPixelShader)
	: myPixelShader{ aPixelShader }
{
}

void FullscreenPass::Draw() const
{
	if (!operator bool())
		return;

	InputLayoutManager::Get().SetInputLayout(typeid(EmptyVertex));
	ScopedShader vertexShader{ VERTEX_SHADER("VsFullscreen.cso") };
	ScopedShader pixelShader{ myPixelShader };

	DX11_CONTEXT->Draw(3, 0);
}
