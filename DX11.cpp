#include "pch.h"
#include "DX11.h"
#pragma comment(lib, "DXGI") 
#pragma comment(lib, "D3D11") 

namespace
{
	ID3D11Device* theDevice = nullptr;
	ID3D11DeviceContext* theContext = nullptr;
}

/*
* namespace DX11
*/

bool DX11::Create()
{
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	std::array levels
	{
		D3D_FEATURE_LEVEL_11_1,
	};

	HRESULT result = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		levels.data(),
		(UINT)levels.size(),
		D3D11_SDK_VERSION,
		&theDevice,
		NULL,
		&theContext
	);

	return SUCCEEDED(result);
}

void DX11::Destroy()
{
	if (theContext) { theContext->Release(); theContext = nullptr; }
	if (theDevice) { theDevice->Release(); theDevice = nullptr; }
}

ID3D11Device* DX11::GetDevice()
{
	return theDevice;
}

ID3D11DeviceContext* DX11::GetContext()
{
	return theContext;
}
