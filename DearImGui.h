#pragma once

namespace DearImGui
{
	// Window::Create() and DX11::Create() must be called before DearImGui::Create()
	bool Create();
	void Destroy();
	void NewFrame();
	void Render();

	struct Scope
	{
		const bool ok;

		Scope() : ok{ Create() } {}
		~Scope() { Destroy(); }
	};
};

