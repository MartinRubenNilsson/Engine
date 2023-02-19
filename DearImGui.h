#pragma once

class DearImGui
{
public:
	DearImGui(HWND, ID3D11Device*, ID3D11DeviceContext*);
	~DearImGui();

	void NewFrame();
	void Render();

	explicit operator bool() const { return mySucceeded; }

private:
	void AddFonts();

	bool mySucceeded{ false };
};
