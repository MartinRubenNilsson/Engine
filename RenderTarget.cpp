#include "pch.h"
#include "RenderTarget.h"

void RenderTarget::SetRenderTarget()
{
	DX11_CONTEXT->OMSetRenderTargets(1, myRenderTargetView.GetAddressOf(), NULL);
}

void RenderTarget::ClearRenderTarget(const float aColor[4])
{
	DX11_CONTEXT->ClearRenderTargetView(myRenderTargetView.Get(), aColor);
}
