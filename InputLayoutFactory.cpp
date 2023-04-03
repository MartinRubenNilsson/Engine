#include "pch.h"
#include "InputLayoutFactory.h"

namespace
{
	template <class VertexType>
	auto CreateInputLayout()
	{
		InputLayoutPtr layout{};

		if (auto shader = ShaderFactory::Get().GetAsset(VertexType::Shader))
		{
			std::span elements{ VertexType::Elements };
			std::string_view bytecode{ shader->GetBytecode() };

			DX11::GetDevice()->CreateInputLayout(
				elements.data(), (UINT)elements.size(),
				bytecode.data(), bytecode.size(),
				&layout
			);
		}

		return layout;
	}

	template <class VertexType>
	auto CreatePair()
	{
		return std::make_pair(std::type_index{ typeid(VertexType) }, CreateInputLayout<VertexType>());
	}

	template <class... VertexTypes>
	auto CreateMap(TypeList<VertexTypes...>)
	{
		return std::unordered_map{ (CreatePair<VertexTypes>(), ...) };
	}
}

InputLayoutFactory::InputLayoutFactory()
	: myInputLayouts{ CreateMap(VertexTypes{}) }
{
}

InputLayoutPtr InputLayoutFactory::GetInputLayout(std::type_index aVertexType) const
{
	auto itr = myInputLayouts.find(aVertexType);
	if (itr != myInputLayouts.end())
		return itr->second;
	return nullptr;
}

InputLayoutFactory::operator bool() const
{
	return std::ranges::all_of(myInputLayouts, &decltype(myInputLayouts)::value_type::second);
}
