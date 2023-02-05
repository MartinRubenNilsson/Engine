#pragma once

class DepthBuffer
{
public:
	DepthBuffer(unsigned aWidth, unsigned aHeight);

	void ClearDepth();

	ID3D11DepthStencilView* GetDepth() const { return myDepth.Get(); }

	operator bool() const { return SUCCEEDED(myResult); }

private:
	ComPtr<ID3D11Texture2D> myTexture;
	ComPtr<ID3D11DepthStencilView> myDepth;
	HRESULT myResult;
};

