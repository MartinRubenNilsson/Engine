#pragma once

namespace Console
{
	bool Create();
	void Destroy();

	struct Scope
	{
		const bool ok;

		Scope() : ok{ Create() } {}
		~Scope() { Destroy(); }
	};

	void SetVisible(bool);
	bool IsVisible();
	void Write(const std::string&);
	std::string Read();
	void SetTitle(const std::string&);
	std::string GetTitle();
}

