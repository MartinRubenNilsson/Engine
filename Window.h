#pragma once

struct WindowClass : WNDCLASS
{
	WindowClass(WNDPROC);
	~WindowClass();
};

class Window
{
public:
	Window(const WNDCLASS&);
	~Window();

	void SetTitle(std::wstring_view);
	std::wstring GetTitle() const;
	RECT GetClientRect() const;
	HWND GetHandle() const { return myWindow; }

	operator bool() const { return myWindow; }

private:
	HWND myWindow;
};

