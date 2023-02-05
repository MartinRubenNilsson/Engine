#pragma once

class FullscreenPass
{
public:
	FullscreenPass(std::shared_ptr<PixelShader>);

	void Draw() const;

	operator bool() const;

private:
	std::shared_ptr<VertexShader> myVertexShader;
	std::shared_ptr<PixelShader> myPixelShader;
};

