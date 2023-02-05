#include "pch.h"
#include "InputLayoutManager.h"
#include "Shader.h"

void InputLayoutManager::SetInputLayout(std::type_index aVertexType) const
{
	auto itr = myInputLayouts.find(aVertexType);
	if (itr != myInputLayouts.end())
		DX11_CONTEXT->IASetInputLayout(itr->second.Get());
}

bool InputLayoutManager::CreateInputLayout(
	std::type_index aVertexType,
	std::span<const D3D11_INPUT_ELEMENT_DESC> someElements,
	const fs::path& aVertexShaderPath
)
{
	if (myInputLayouts.contains(aVertexType))
		return false;

	auto vertexShader = ShaderManager::Get().GetShader<VertexShader>(aVertexShaderPath);
	if (!vertexShader)
		return false;

	const auto& bytecode = vertexShader->GetBytecode();

	ComPtr<ID3D11InputLayout> inputLayout{};

	DX11_DEVICE->CreateInputLayout(
		someElements.data(),
		static_cast<UINT>(someElements.size()),
		bytecode.data(),
		bytecode.size(),
		&inputLayout
	);

	if (!inputLayout)
		return false;

	myInputLayouts.emplace(aVertexType, inputLayout);
	return true;
}
