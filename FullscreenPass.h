#pragma once

class FullscreenPass
{
public:
	FullscreenPass() = default;
	FullscreenPass(const fs::path& aPixelShader);

	void Render() const;

private:
	ScopedLayout myLayout;
	ScopedShader myVertexShader, myPixelShader;
};

