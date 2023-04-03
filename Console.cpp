#include "pch.h"
#include "Console.h"

bool Console::Create()
{
	return AllocConsole();
}

bool Console::Destroy()
{
	return FreeConsole();
}

bool Console::IsCreated()
{
	return GetConsoleWindow();
}

void Console::Write(const std::string& aString)
{
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), aString.c_str(), (DWORD)aString.size(), NULL, NULL);
}

std::string Console::Read()
{
	CHAR buffer[1024]{};
	DWORD charsRead = 0;
	ReadConsoleA(GetStdHandle(STD_INPUT_HANDLE), buffer, 1024, &charsRead, NULL);
	return { buffer, charsRead };
}

void Console::SetTitle(std::string_view aString)
{
	SetConsoleTitleA(aString.data());
}

std::string Console::GetTitle()
{
	CHAR title[256]{};
	DWORD length = GetConsoleTitleA(title, 256);
	return { title, length };
}
