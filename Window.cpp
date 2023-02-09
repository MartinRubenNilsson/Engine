#include "pch.h"
#include "Window.h"


// WindowClass

WindowClass::WindowClass(WNDPROC aWndProc)
	: WNDCLASS{}
{
	lpfnWndProc = aWndProc;
	hInstance = GetModuleHandle(NULL);
	lpszClassName = L"Window";

	RegisterClass(this);
}

WindowClass::~WindowClass()
{
	UnregisterClass(lpszClassName, hInstance);
}


// Window

Window::Window(const WNDCLASS& aClass)
	: myWindow{ CreateWindowEx(
		WS_EX_ACCEPTFILES,
		aClass.lpszClassName,
		NULL,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		aClass.hInstance,
		NULL
	), DestroyWindow }
{
}

void Window::SetTitle(std::wstring_view aString)
{
	SetWindowText(myWindow.get(), aString.data());
}

std::wstring Window::GetTitle() const
{
	std::wstring title{};
	title.resize(GetWindowTextLength(myWindow.get()) + 1);
	title.resize(GetWindowText(myWindow.get(), title.data(), static_cast<int>(title.size())));
	return title;
}

RECT Window::GetClientRect() const
{
	RECT rect{};
	::GetClientRect(myWindow.get(), &rect);
	return rect;
}
