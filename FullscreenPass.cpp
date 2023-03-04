#include "pch.h"
#include "FullscreenPass.h"

FullscreenPass::FullscreenPass(std::shared_ptr<const Shader> aPixelShader)
	: myPixelShader{ aPixelShader }
{
}

void FullscreenPass::Render() const
{
	if (!operator bool())
		return;

	ScopedInputLayout scopedLayout{ typeid(EmptyVertex) };
	ScopedShader scopedVs{ GET_SHADER("VsFullscreenTriangle.cso") };
	ScopedShader scopedPs{ myPixelShader };

	DX11_CONTEXT->Draw(3, 0);
}

FullscreenPass::operator bool() const
{
	return myPixelShader && *myPixelShader && myPixelShader->GetType() == ShaderType::Pixel;
}
