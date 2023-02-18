#pragma once

class Window
{
public:
	static ATOM Register(WNDPROC);

	Window();
	Window(ATOM);

	void SetTitle(std::wstring_view);
	std::wstring GetTitle() const;
	RECT GetClientRect() const;

	operator HWND() const { return myWindow.get(); }
	explicit operator bool() const;

private:
	using Element = std::remove_pointer_t<HWND>;
	using Deleter = decltype(DestroyWindow);

	std::unique_ptr<Element, Deleter*> myWindow;
};

