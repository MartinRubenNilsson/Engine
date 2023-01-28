#include "pch.h"
#include "Viewport.h"

void Viewport::SetViewport() const
{
	DX11_CONTEXT->RSSetViewports(1, this);
}
