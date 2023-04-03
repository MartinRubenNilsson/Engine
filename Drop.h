#pragma once

namespace Drop
{
	// Call this in the window procedure
	void Accept(HDROP);

	// Call as such: if (Begin()) { ...; End(); }
	bool Begin();
	void End();

	std::vector<fs::path> GetPaths(); // Only call inbetween Begin()/End()
	POINT GetPoint(); // Only call inbetween Begin()/End()
}

