#pragma once

class FullscreenPass
{
public:
	FullscreenPass(std::shared_ptr<PixelShader>);

	void Draw() const;

	operator bool() const { return myPixelShader.operator bool(); };

private:
	std::shared_ptr<PixelShader> myPixelShader;
};

