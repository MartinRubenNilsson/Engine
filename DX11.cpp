#include "pch.h"
#include "DX11.h"
#pragma comment(lib, "DXGI") 
#pragma comment(lib, "D3D11") 

DX11* DX11::ourInstance = nullptr;

DX11::DX11()
{
	assert(!ourInstance);
	ourInstance = this;

	myResult = D3D11CreateDevice(
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
}

DX11::~DX11()
{
	ourInstance = nullptr;
}
