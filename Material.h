#pragma once
#include "Texture.h"

class Material
{
public:
	Material() = default;
	Material(const aiMaterial&);

	std::string_view GetName() const { return myName; }
	const auto& GetTextures() const { return myTextures; }

private:
	std::string myName{};
	std::vector<Texture::Ptr> myTextures{};
};

namespace ImGui
{
	void Inspect(const Material&);
}

