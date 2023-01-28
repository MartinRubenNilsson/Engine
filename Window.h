#pragma once

struct WindowClass : WNDCLASS
{
	WindowClass(WNDPROC);
	~WindowClass();
};

class Window
{
public:
	Window(const WindowClass&);
	~Window();

	void SetTitle(std::wstring_view);
	std::wstring GetTitle() const;

	LONG GetClientWidth() const;
	LONG GetClientHeight() const;

	HWND GetHandle() const { return myWindow; }
	operator bool() const { return myWindow; }

private:
	HWND myWindow;
};

