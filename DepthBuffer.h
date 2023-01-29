#pragma once

class DepthBuffer
{
public:
	DepthBuffer(unsigned aWidth, unsigned aHeight);

	ID3D11DepthStencilView* GetDepthStencilView() const { return myDepthStencilView.Get(); }

	operator bool() const { return SUCCEEDED(myResult); }

private:
	HRESULT myResult;
	ComPtr<ID3D11Texture2D> myTexture2d;
	ComPtr<ID3D11DepthStencilView> myDepthStencilView;
};

