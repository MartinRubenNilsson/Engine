#pragma once

/*
* Base class
*/

class InputLayout
{
public:
	virtual ~InputLayout() = default;
	void SetInputLayout() const;

	operator bool() const { return myInputLayout; }

protected:
	InputLayout(std::string_view someShaderBytecode, std::initializer_list<D3D11_INPUT_ELEMENT_DESC>);

	ComPtr<ID3D11InputLayout> myInputLayout;
};

/*
* Derived classes
*/

class BasicInputLayout : public InputLayout
{
public:
	BasicInputLayout(std::string_view someShaderBytecode);
};
