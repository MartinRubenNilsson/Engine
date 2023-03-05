#include "pch.h"
#include "InputLayoutManager.h"

namespace
{
	auto CreateInputLayout(std::span<const D3D11_INPUT_ELEMENT_DESC> someElements, std::string_view someBytecode)
	{
		InputLayoutPtr layout{};
		DX11_DEVICE->CreateInputLayout(
			someElements.data(), (UINT)someElements.size(),
			someBytecode.data(), someBytecode.size(),
			&layout
		);

		return layout;
	}

	std::string LoadBytecode(const fs::path& aPath)
	{
		std::ifstream file{ aPath, std::ios::binary };
		return { std::istreambuf_iterator{ file }, {} };
	}

	template <class VertexType>
	auto CreateInputLayout()
	{
		std::string bytecode = LoadBytecode(VertexType::Shader);
		return !bytecode.empty() ? CreateInputLayout(VertexType::Elements, bytecode) : nullptr;
	}
}

#define PAIR(VertexType) { typeid(VertexType), CreateInputLayout<VertexType>() }

InputLayoutManager::InputLayoutManager()
	: myInputLayouts
	{
		PAIR(EmptyVertex),
		PAIR(VsInBasic),
	}
{}

void InputLayoutManager::SetInputLayout(std::type_index aVertexType) const
{
	auto itr = myInputLayouts.find(aVertexType);
	if (itr != myInputLayouts.end())
		DX11_CONTEXT->IASetInputLayout(itr->second.Get());
}

InputLayoutManager::operator bool() const
{
	return std::ranges::all_of(myInputLayouts, &decltype(myInputLayouts)::value_type::second);
}
