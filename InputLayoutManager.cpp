#include "pch.h"
#include "InputLayoutManager.h"

InputLayoutManager::InputLayoutManager()
{
	{
		D3D11_INPUT_ELEMENT_DESC desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		myInputLayouts.emplace(typeid(BasicVertex), InputLayout{ desc, LoadShaderBytecode("VsBasic.cso") });
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
	bool succeeded{ true };
	for (auto& [vertexType, inputLayout] : myInputLayouts)
		succeeded = succeeded && inputLayout;
	return succeeded;
}

std::string InputLayoutManager::LoadShaderBytecode(const fs::path& aPath)
{
	std::ifstream file{ aPath, std::ios::binary };
	return { std::istreambuf_iterator{ file }, {} };
}
