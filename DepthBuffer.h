#pragma once

class DepthBuffer
{
public:
	DepthBuffer(unsigned aWidth, unsigned aHeight);

	void ClearDepthStencil();

	ID3D11DepthStencilView* GetDepthStencil() const { return myDepthStencil.Get(); }

	operator bool() const { return SUCCEEDED(myResult); }

private:
	ComPtr<ID3D11Texture2D> myTexture;
	ComPtr<ID3D11DepthStencilView> myDepthStencil;
	HRESULT myResult;
};

