#include "pch.h"
#include "DX11.h"
#pragma comment(lib, "DXGI") 
#pragma comment(lib, "D3D11") 

DX11* DX11::Ptr = nullptr;

DX11::DX11()
{
	assert(!Ptr);

	HRESULT result = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&myDevice,
		NULL,
		&myContext
	);

	if (SUCCEEDED(result))
		Ptr = this;
}
