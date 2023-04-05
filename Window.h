#pragma once

namespace Window
{
	bool Create();
	void Destroy();
	void SetIcon(const fs::path&);
	void SetTitle(const std::string&);
	std::string GetTitle();
	RECT GetClientRect();
	HWND GetHandle();

	struct Scope
	{
		const bool ok;

		Scope() : ok{ Create() } {}
		~Scope() { Destroy(); }
	};
}

