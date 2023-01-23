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

Window::Window(const WindowClass& aClass)
	: myWindow{}
{
	myWindow = CreateWindow(
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
	);
}

Window::~Window()
{
	DestroyWindow(myWindow);
}

void Window::SetTitle(std::wstring_view aString)
{
	SetWindowText(myWindow, aString.data());
}

std::wstring Window::GetTitle() const
{
	std::wstring title{};
	title.resize(GetWindowTextLength(myWindow) + 1);
	title.resize(GetWindowText(myWindow, title.data(), title.size()));
	return title;
}
