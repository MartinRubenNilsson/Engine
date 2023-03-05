#pragma once

class InputLayoutManager : public Singleton<InputLayoutManager>
{
public:
	InputLayoutManager();

	void SetInputLayout(std::type_index aVertexType) const;

	explicit operator bool() const;

private:
	std::unordered_map<std::type_index, InputLayoutPtr> myInputLayouts;
};

