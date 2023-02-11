#pragma once

class FullscreenPass
{
public:
	FullscreenPass(std::shared_ptr<const PixelShader>);

	void Draw() const;

	explicit operator bool() const { return myPixelShader.operator bool(); };

private:
	std::shared_ptr<const PixelShader> myPixelShader;
};

