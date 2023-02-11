#pragma once

class DX11 : public Singleton<DX11>
{
public:
	DX11();

	ID3D11Device* GetDevice() const { return myDevice.Get(); }
	ID3D11DeviceContext* GetContext() const { return myContext.Get(); }

	explicit operator bool() const { return SUCCEEDED(myResult); }

private:
	HRESULT myResult{ E_FAIL };
	ComPtr<ID3D11Device> myDevice{};
	ComPtr<ID3D11DeviceContext> myContext{};
};

#define DX11_DEVICE DX11::Get().GetDevice()
#define DX11_CONTEXT DX11::Get().GetContext()
