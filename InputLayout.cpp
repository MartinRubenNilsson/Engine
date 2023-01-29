#include "pch.h"
#include "InputLayout.h"

/*
* Base class
*/

InputLayout::InputLayout(std::string_view someShaderBytecode, std::initializer_list<D3D11_INPUT_ELEMENT_DESC> someInputElements)
{
	DX11_DEVICE->CreateInputLayout(
		someInputElements.begin(),
		(UINT)someInputElements.size(),
		someShaderBytecode.data(),
		someShaderBytecode.size(),
		&myInputLayout
	);
}

void InputLayout::SetInputLayout() const
{
	DX11_CONTEXT->IASetInputLayout(myInputLayout.Get());
}

/*
* Derived classes
*/

BasicInputLayout::BasicInputLayout(std::string_view someShaderBytecode) : InputLayout{ someShaderBytecode,
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	}}
{
}
