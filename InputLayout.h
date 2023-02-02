#pragma once

class InputLayout
{
public:
	InputLayout(
		std::span<D3D11_INPUT_ELEMENT_DESC> someInputElements,
		std::string_view someShaderBytecode
	);
	
	void SetInputLayout() const;

	operator bool() const { return myInputLayout; }

private:
	ComPtr<ID3D11InputLayout> myInputLayout;
};

