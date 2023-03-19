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

void GetPath(MenuCommand&, fs::path&);

namespace ImGui
{
	void Shortcut(MenuCommand&);
	void FileMenu(MenuCommand&);
	void MainMenu(MenuCommand&);
}

