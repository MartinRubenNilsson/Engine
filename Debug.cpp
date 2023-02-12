#include "pch.h"
#include "Debug.h"

void Debug::Print(std::string_view aString)
{
	OutputDebugStringA(aString.data());
}

void Debug::Println(std::string_view aString)
{
	Print(aString);
	Print("\n");
}
