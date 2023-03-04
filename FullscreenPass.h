#pragma once

class FullscreenPass
{
public:
	FullscreenPass() = default;
	FullscreenPass(std::shared_ptr<const Shader> aPixelShader);

	void Render() const;

	explicit operator bool() const;

private:
	std::shared_ptr<const Shader> myPixelShader{};
};

