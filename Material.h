#pragma once
#include "Texture.h"

class Material
{
public:
	Material();
	Material(const aiMaterial&);

	void SetTexture(Texture::Ptr);
	Texture::Ptr GetTexture(TextureType) const;

	std::vector<ShaderResourcePtr> GetResources() const;

	std::string_view GetName() const { return myName; }

private:
	friend void to_json(json&, const Material&);
	friend void from_json(const json&, Material&);

	std::string myName{ "Default" };
	std::vector<Texture::Ptr> myTextures{};
};

namespace ImGui
{
	void Inspect(const Material&);
}

