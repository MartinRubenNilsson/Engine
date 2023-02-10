#pragma once

class Material
{
public:
	Material(const aiMaterial&);

	operator std::span<const ShaderResourcePtr>() const { return myShaderResources; }

private:
	enum TextureType
	{
		Diffuse,
		Count
	};

	std::string myName;
	TexturePtr myTextures[Count];
	ShaderResourcePtr myShaderResources[Count];
};

