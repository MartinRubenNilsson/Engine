#pragma once

namespace Console
{
	bool Create();
	bool Destroy();
	bool IsCreated();

	void Write(const std::string&);
	std::string Read();

	void SetTitle(std::string_view);
	std::string GetTitle();
}

