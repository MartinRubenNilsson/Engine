#include "pch.h"
#include "InputLayout.h"

InputLayout::InputLayout(
	std::span<D3D11_INPUT_ELEMENT_DESC> someInputElements,
	std::string_view someShaderBytecode
)
{
	DX11_DEVICE->CreateInputLayout(
		someInputElements.data(),
		static_cast<UINT>(someInputElements.size()),
		someShaderBytecode.data(),
		someShaderBytecode.size(),
		&myInputLayout
	);
}

void InputLayout::SetInputLayout() const
{
	DX11_CONTEXT->IASetInputLayout(myInputLayout.Get());
}
