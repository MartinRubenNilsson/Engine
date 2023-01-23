#pragma once
#include "RenderTarget.h"

class SwapChain : public RenderTarget
{
public:
	SwapChain(const Window&);

	operator bool() const { return mySwapChain; }

	void Present();

private:
	ComPtr<IDXGISwapChain> mySwapChain;
};

