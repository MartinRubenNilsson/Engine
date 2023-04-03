#pragma once

namespace DearImGui
{
	bool Create(HWND, ID3D11Device*, ID3D11DeviceContext*);
	void NewFrame();
	void Render();
};

