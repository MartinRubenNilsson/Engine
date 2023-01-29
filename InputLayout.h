#pragma once

class InputLayout
{
public:
	InputLayout(std::string_view someShaderBytecode);
	void SetInputLayout() const;

	operator bool() const { return myInputLayout; }

private:
	ComPtr<ID3D11InputLayout> myInputLayout;
};
