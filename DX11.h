#pragma once

class DX11
{
public:
	static DX11* Instance;

	DX11();

	operator bool() const { return SUCCEEDED(myResult); }

	ID3D11Device* GetDevice() const { return myDevice.Get(); }
	ID3D11DeviceContext* GetContext() const { return myContext.Get(); }

private:
	ComPtr<ID3D11Device> myDevice;
	ComPtr<ID3D11DeviceContext> myContext;
	HRESULT myResult;
};

#define DX11_DEVICE DX11::Instance->GetDevice()
#define DX11_CONTEXT DX11::Instance->GetContext()
