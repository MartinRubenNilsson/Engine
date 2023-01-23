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

	operator bool() const { return myWindow; }

	void SetTitle(std::wstring_view);
	std::wstring GetTitle() const;

	HWND GetHandle() const { return myWindow; }

private:
	HWND myWindow;
};

