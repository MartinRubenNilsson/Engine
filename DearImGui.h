#pragma once

class DearImGui
{
public:
	DearImGui(HWND, ID3D11Device*, ID3D11DeviceContext*);
	~DearImGui();

	void NewFrame();
	void Render();

	explicit operator bool() const;

private:
	void AddFonts();

	bool mySucceeded{ false };
};

