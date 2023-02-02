#pragma once
#include "InputLayout.h"

struct BasicVertex
{
	Vector3 position;
	Vector3 normal;
};

class InputLayoutManager : public Singleton<InputLayoutManager>
{
public:
	InputLayoutManager();

	void SetInputLayout(std::type_index aVertexType) const;

	operator bool() const;

private:
	static std::string LoadShaderBytecode(const fs::path&);

	std::unordered_map<std::type_index, InputLayout> myInputLayouts;
};

#define DX11_SET_INPUT_LAYOUT(aVertexType) InputLayoutManager::Get().SetInputLayout(typeid(aVertexType))

