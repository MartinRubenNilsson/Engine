#pragma once

class DearImGui
{
public:
	DearImGui(HWND, ID3D11Device*, ID3D11DeviceContext*);
	~DearImGui();

	operator bool() const { return mySucceeded; }

	void NewFrame();
	void Render();

private:
	bool mySucceeded;
};
