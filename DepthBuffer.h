#pragma once

class DepthBuffer
{
public:
	DepthBuffer(unsigned aWidth, unsigned aHeight);

	void ClearDepthStencil();

	operator ComPtr<ID3D11DepthStencilView>() const { return myDepthStencil; }

	operator bool() const { return SUCCEEDED(myResult); }

private:
	HRESULT myResult;
	ComPtr<ID3D11Texture2D> myTexture;
	ComPtr<ID3D11DepthStencilView> myDepthStencil;
};

