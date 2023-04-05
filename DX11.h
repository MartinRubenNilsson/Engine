#pragma once

namespace DX11
{
	bool Create();
	void Destroy();
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();

	struct Scope
	{
		const bool ok;

		Scope() : ok{ Create() } {}
		~Scope() { Destroy(); }
	};
}

