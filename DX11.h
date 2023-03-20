#pragma once

class DX11 : public Singleton<DX11>
{
public:
	DX11();

	DevicePtr GetDevice() const { return myDevice; }
	DeviceContextPtr GetContext() const { return myContext; }

	explicit operator bool() const;

private:
	DevicePtr myDevice{};
	DeviceContextPtr myContext{};
	HRESULT myResult{ E_FAIL };
};

#define DX11_DEVICE DX11::Get().GetDevice().Get()
#define DX11_CONTEXT DX11::Get().GetContext().Get()
