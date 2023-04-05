#include "pch.h"
#include "Window.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

namespace
{
	HWND theHandle = NULL;
}

/*
* namespace Window
*/

bool Window::Create()
{
	WNDCLASS wndClass{};
	wndClass.style = 0;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.hIcon = NULL;
	wndClass.hCursor = NULL;
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"Window";

	if (RegisterClass(&wndClass) == 0)
		return false;

	theHandle = CreateWindowEx(
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
	);

	return theHandle;
}

void Window::Destroy()
{
	DestroyWindow(theHandle);
}

void Window::SetIcon(const fs::path& aPath)
{
	if (HICON hIcon = (HICON)LoadImage(NULL, aPath.wstring().c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE))
	{
		SendMessage(theHandle, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		SendMessage(theHandle, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	}
}

void Window::SetTitle(const std::string& aString)
{
	SetWindowTextA(theHandle, aString.c_str());
}

std::string Window::GetTitle()
{
	CHAR title[MAX_PATH]{};
	GetWindowTextA(theHandle, title, MAX_PATH);
	return title;
}

RECT Window::GetClientRect()
{
	RECT rect{};
	GetClientRect(theHandle, &rect);
	return rect;
}

HWND Window::GetHandle()
{
	return theHandle;
}
