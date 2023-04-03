#pragma once

namespace Window
{
	bool Create();

	void SetIcon(const fs::path&);

	void SetTitle(const std::string&);
	std::string GetTitle();

	RECT GetClientRect();

	HWND GetHandle();
}

