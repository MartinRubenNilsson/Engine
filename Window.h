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

	void SetTitle(std::wstring_view);
	std::wstring GetTitle() const;
	RECT GetClientRect() const;
	HWND GetHandle() const { return myWindow.get(); }

	operator bool() const { return myWindow.operator bool(); }

private:
	using Element = std::remove_pointer_t<HWND>;
	using Deleter = decltype(DestroyWindow);

	std::unique_ptr<Element, Deleter*> myWindow;
};

