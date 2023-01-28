#pragma once
#include "RenderTarget.h"

class Window;

class SwapChain : public RenderTarget
{
public:
	SwapChain(const Window&);

	operator bool() const { return mySwapChain; }

	void Present();

private:
	ComPtr<IDXGISwapChain> mySwapChain;
};

