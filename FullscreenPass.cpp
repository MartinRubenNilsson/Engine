#include "pch.h"
#include "FullscreenPass.h"

FullscreenPass::FullscreenPass(std::shared_ptr<PixelShader> aPixelShader)
	: myVertexShader{ DX11_VERTEX_SHADER(VertexTraits<FullscreenVertex>::shader)}
	, myPixelShader{ aPixelShader }
{
}

void FullscreenPass::Draw() const
{
	if (!operator bool())
		return;

	DX11_SET_INPUT_LAYOUT(typeid(FullscreenVertex));

	myVertexShader->SetShader();
	myPixelShader->SetShader();

	DX11_CONTEXT->Draw(3, 0);
}

FullscreenPass::operator bool() const
{
	return myVertexShader && *myVertexShader && myPixelShader && *myPixelShader;
}
