#pragma once

class DearImGui
{
public:
	DearImGui(HWND, ID3D11Device*, ID3D11DeviceContext*);
	~DearImGui();

	void NewFrame();
	void Render();

	operator bool() const { return mySucceeded; }

private:
	bool mySucceeded;
};
