#pragma once
#include "Texture.h"

class Material
{
public:
	Material();
	Material(const aiMaterial&);

	void AddOrReplaceTexture(Texture::Ptr);

	ShaderResourcePtr GetResource(TextureType) const;

	std::string_view GetName() const { return myName; }
	const auto& GetTextures() const { return myTextures; }

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

