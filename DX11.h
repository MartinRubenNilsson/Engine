#pragma once

class DX11
{
public:
	static DX11* Ptr;

	DX11();

	operator bool() const { return myDevice && myContext; }

	ID3D11Device* GetDevice() const { return myDevice.Get(); }
	ID3D11DeviceContext* GetContext() const { return myContext.Get(); }

private:
	ComPtr<ID3D11Device> myDevice;
	ComPtr<ID3D11DeviceContext> myContext;
};

#define DX11_DEVICE DX11::Ptr->GetDevice()
#define DX11_CONTEXT DX11::Ptr->GetContext()
