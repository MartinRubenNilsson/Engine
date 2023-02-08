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

	auto vertexShader{ VERTEX_SHADER("VsFullscreen.cso")};
	if (!vertexShader)
		return;

	InputLayoutManager::Get().SetInputLayout(typeid(EmptyVertex));

	vertexShader->SetShader();
	myPixelShader->SetShader();

	DX11_CONTEXT->Draw(3, 0);
}
