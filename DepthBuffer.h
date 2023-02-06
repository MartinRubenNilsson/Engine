#pragma once

class DepthBuffer
{
public:
	DepthBuffer(unsigned aWidth, unsigned aHeight);

	void ClearDepthStencil();

	operator ID3D11DepthStencilView* const() const { return myDepthStencil.Get(); }

	operator bool() const { return SUCCEEDED(myResult); }

private:
	HRESULT myResult;
	ComPtr<ID3D11Texture2D> myTexture;
	ComPtr<ID3D11DepthStencilView> myDepthStencil;
};

