#pragma once

enum class FileCommand
{
	None,
	NewScene,
	OpenScene,
	Save,
	SaveAs,
	Exit,
};

namespace ImGui
{
	FileCommand FileMenu(fs::path&);
}

