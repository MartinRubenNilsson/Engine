#include "pch.h"
#include "InputLayout.h"

InputLayout::InputLayout(std::string_view someShaderBytecode)
{
	D3D11_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	DX11_DEVICE->CreateInputLayout(
		inputElementDescs,
		static_cast<UINT>(std::size(inputElementDescs)),
		someShaderBytecode.data(),
		someShaderBytecode.size(),
		&myInputLayout
	);
}

void InputLayout::SetInputLayout() const
{
	DX11_CONTEXT->IASetInputLayout(myInputLayout.Get());
}
