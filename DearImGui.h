#pragma once

class DearImGui
{
public:
	DearImGui(HWND, DevicePtr, DeviceContextPtr);
	~DearImGui();

	void NewFrame();
	void Render();

	explicit operator bool() const;

private:
	void AddFonts();

	bool mySucceeded{ false };
};
