#pragma once

class FullscreenPass
{
public:
	FullscreenPass(std::shared_ptr<PixelShader>);

	void Draw() const;

	operator bool() const;

private:
	static constexpr char ourVertexShader[] = "VsFullscreen.cso";

	std::shared_ptr<VertexShader> myVertexShader;
	std::shared_ptr<PixelShader> myPixelShader;
};

