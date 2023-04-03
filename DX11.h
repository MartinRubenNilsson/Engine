#pragma once

namespace DX11
{
	bool Create();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
}

