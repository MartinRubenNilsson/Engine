#pragma once

class DX11
{
public:
	DX11();
	~DX11();

	static DX11& Get() { return *ourInstance; }

	ID3D11Device* GetDevice() const { return myDevice.Get(); }
	ID3D11DeviceContext* GetContext() const { return myContext.Get(); }

	operator bool() const { return SUCCEEDED(myResult); }

private:
	DX11(const DX11&) = delete;
	DX11& operator=(const DX11&) = delete;
	DX11(DX11&&) = delete;
	DX11& operator=(DX11&&) = delete;

	static DX11* ourInstance;

	HRESULT myResult;
	ComPtr<ID3D11Device> myDevice;
	ComPtr<ID3D11DeviceContext> myContext;
};

#define DX11_DEVICE DX11::Get().GetDevice()
#define DX11_CONTEXT DX11::Get().GetContext()
