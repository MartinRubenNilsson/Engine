#include "pch.h"
#include "FullscreenPass.h"

FullscreenPass::FullscreenPass(std::shared_ptr<PixelShader> aPixelShader)
	: myVertexShader{ ShaderManager::Get().GetShader<VertexShader>(ourVertexShader) }
	, myPixelShader{ aPixelShader }
{
}

void FullscreenPass::Draw() const
{
	if (!operator bool())
		return;

	myVertexShader->SetShader();
	myPixelShader->SetShader();

	DX11_CONTEXT->DrawIndexed(3, 0, 0);
}

FullscreenPass::operator bool() const
{
	return myVertexShader && *myVertexShader && myPixelShader && *myPixelShader;
}
