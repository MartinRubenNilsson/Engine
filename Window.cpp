#include "pch.h"
#include "Window.h"

Window::Window()
	: myHandle{ nullptr, DestroyWindow }
{
}

Window::Window(WNDPROC aWndProc)
	: Window{}
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

	if (RegisterClass(&wndClass) == 0)
		return;

	myHandle.reset(CreateWindowEx(
		WS_EX_ACCEPTFILES,
		wndClass.lpszClassName,
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

void Window::SetIcon(const fs::path& aPath)
{
	if (HICON hIcon = (HICON)LoadImage(NULL, aPath.wstring().c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE))
	{
		SendMessage(myHandle.get(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		SendMessage(myHandle.get(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	}
}

void Window::SetTitle(std::wstring_view aString)
{
	SetWindowText(myHandle.get(), aString.data());
}

std::wstring Window::GetTitle() const
{
	std::wstring title{};
	title.resize(GetWindowTextLength(myHandle.get()) + 1);
	title.resize(GetWindowText(myHandle.get(), title.data(), static_cast<int>(title.size())));
	return title;
}

RECT Window::GetClientRect() const
{
	RECT rect{};
	::GetClientRect(myHandle.get(), &rect);
	return rect;
}

Window::operator HWND() const
{
	return myHandle.get();
}

Window::operator bool() const
{
	return myHandle.operator bool();
}
