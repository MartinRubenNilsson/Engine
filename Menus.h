#pragma once

enum class MenuCommand
{
	None,

	// File
	NewScene,
	OpenScene,
	Save,
	SaveAs,
	Exit,
};

namespace ImGui
{
	void Shortcut(MenuCommand&);

	void FileMenu(MenuCommand&);
	void MainMenu(MenuCommand&);

	void GetPath(MenuCommand&, fs::path&);
}

