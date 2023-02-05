#include "pch.h"
#include "InputLayoutManager.h"
#include "Shader.h"

InputLayoutManager::InputLayoutManager()
{
	{
		D3D11_INPUT_ELEMENT_DESC desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		if (auto shader = ShaderManager::Get().GetShader<VertexShader>("VsBasic.cso"))
			myInputLayouts.emplace(typeid(BasicVertex), InputLayout{ desc, shader->GetBytecode() });
	}
}

void InputLayoutManager::SetInputLayout(std::type_index aVertexType) const
{
	auto itr = myInputLayouts.find(aVertexType);
	if (itr != myInputLayouts.end())
		itr->second.SetInputLayout();
}

InputLayoutManager::operator bool() const
{
	for (auto& [vertexType, inputLayout] : myInputLayouts)
	{
		if (!inputLayout)
			return false;
	}
	return true;
}
