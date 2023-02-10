#pragma once

class Material
{
public:
	Material(const aiMaterial&);

private:
	void CreateShaderResources();

	enum TextureType
	{
		Diffuse,
		Count
	};

	std::string myName;
	ComPtr<ID3D11Texture2D> myTextures[Count];
	ComPtr<ID3D11ShaderResourceView> myShaderResources[Count];
};

