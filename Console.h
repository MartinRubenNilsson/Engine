#pragma once

namespace Console
{
	bool Create();
	bool Destroy();
	bool IsCreated();

	void Write(const std::string&);
	std::string Read(); // At most MAX_PATH characters right now

	void SetTitle(const std::string&);
	std::string GetTitle();
}

