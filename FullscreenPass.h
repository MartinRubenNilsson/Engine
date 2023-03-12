#pragma once

class FullscreenPass
{
public:
	FullscreenPass() = default;
	FullscreenPass(const fs::path& aPixelShader);

	void Render() const;

private:
	ScopedInputLayout myLayout;
	ScopedShader myVertexShader, myPixelShader;
};

