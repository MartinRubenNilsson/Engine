#include "pch.h"
#include "DX11.h"
#pragma comment(lib, "DXGI") 
#pragma comment(lib, "D3D11") 

DX11::DX11()
{
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	std::array levels
	{
		D3D_FEATURE_LEVEL_11_1,
	};

	myResult = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		levels.data(),
		(UINT)levels.size(),
		D3D11_SDK_VERSION,
		&myDevice,
		NULL,
		&myContext
	);
}

ID3D11Device* DX11::GetDevice() const
{
	return myDevice.Get();
}

ID3D11DeviceContext* DX11::GetContext() const
{
	return myContext.Get();
}

DX11::operator bool() const
{
	return SUCCEEDED(myResult);
}
