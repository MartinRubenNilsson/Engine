#pragma once

class InputLayoutFactory : public Singleton<InputLayoutFactory>
{
public:
	InputLayoutFactory();

	InputLayoutPtr GetInputLayout(std::type_index aVertexType) const;

	explicit operator bool() const;

private:
	std::unordered_map<std::type_index, InputLayoutPtr> myInputLayouts;
};

