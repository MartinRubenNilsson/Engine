#include "pch.h"
#include "Console.h"

bool Console::Create()
{
	bool ok = AllocConsole();
	if (ok)
	{
		HWND handle = GetConsoleWindow();
		SetWindowLong(handle, GWL_STYLE, GetWindowLong(handle, GWL_STYLE) & ~WS_SYSMENU);
		SetTitle("Console");
	}
	return ok;
}

void Console::Destroy()
{
	FreeConsole();
}

void Console::SetVisible(bool yes)
{
	ShowWindow(GetConsoleWindow(), yes ? SW_SHOW : SW_HIDE);
}

bool Console::IsVisible()
{
	return IsWindowVisible(GetConsoleWindow());
}

void Console::Write(const std::string& aString)
{
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), aString.c_str(), (DWORD)aString.size(), NULL, NULL);
}

std::string Console::Read()
{
	CHAR buffer[MAX_PATH]{};
	DWORD charsRead = 0;
	ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), buffer, MAX_PATH, &charsRead, NULL);
	return { buffer, charsRead };
}

void Console::SetTitle(const std::string& aString)
{
	SetConsoleTitleA(aString.c_str());
}

std::string Console::GetTitle()
{
	CHAR title[MAX_PATH]{};
	DWORD length = GetConsoleTitleA(title, MAX_PATH);
	return { title, length };
}
