#pragma once

class DearImGui
{
public:
	DearImGui(const Window&, const DX11&);
	~DearImGui();

	void NewFrame();
	void Render();
};
