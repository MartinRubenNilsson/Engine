#include "pch.h"
#include "Window.h"

ATOM Window::Register(WNDPROC aWndProc)
{
	WNDCLASS wndClass{};
	wndClass.style = 0;
	wndClass.lpfnWndProc = aWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = NULL;
	wndClass.hCursor = NULL;
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"Window";

	return RegisterClass(&wndClass);
}

Window::Window()
	: myWindow{ nullptr, DestroyWindow }
{
}

Window::Window(ATOM anAtom)
	: Window{}
{
	myWindow.reset(CreateWindowEx(
		WS_EX_ACCEPTFILES,
		MAKEINTATOM(anAtom),
		NULL,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL
	));
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

Window::operator bool() const
{
	return myWindow.operator bool();
}
