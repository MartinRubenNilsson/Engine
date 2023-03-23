#include "pch.h"
#include "Win32Common.h"

fs::path GetExePath()
{
    WCHAR path[MAX_PATH];
    GetModuleFileName(GetModuleHandle(NULL), path, MAX_PATH);
    return { path };
}
