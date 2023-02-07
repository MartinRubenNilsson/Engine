#include "pch.h"
#include "FullscreenPass.h"

FullscreenPass::FullscreenPass(std::shared_ptr<PixelShader> aPixelShader)
	: myPixelShader{ aPixelShader }
{
}

void FullscreenPass::Draw() const
{
	if (!operator bool())
		return;

	auto vertexShader{ VERTEX_SHADER(VertexTraits<FullscreenVertex>::shader) };
	if (!vertexShader)
		return;

	DX11_SET_INPUT_LAYOUT(typeid(FullscreenVertex));

	vertexShader->SetShader();
	myPixelShader->SetShader();

	DX11_CONTEXT->Draw(3, 0);
}
