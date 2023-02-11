#pragma once
#include "Image.h"

class Material
{
public:
	Material(const aiMaterial&);

	operator std::span<const ShaderResourcePtr>() const { return myShaderResources; }

private:
	enum TextureType : size_t
	{
		Diffuse,
		Emissive,
		Normal,
		Metallic,
		Roughness,
		Count
	};

	static constexpr std::array<unsigned, Count> ourChannels
	{
		4, // Diffuse
		4, // Emissive
		4, // Normal
		1, // Metallic
		1, // Roughness
	};

	static constexpr std::array<DXGI_FORMAT, Count> ourFormats
	{
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, // Diffuse
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, // Emissive
		DXGI_FORMAT_R8G8B8A8_UNORM, // Normal
		DXGI_FORMAT_R8_UNORM, // Metallic
		DXGI_FORMAT_R8_UNORM, // Roughness
	};

	void LoadPaths(const aiMaterial&);
	void LoadImages();
	void CreateTextures();
	void CreateShaderResources();

	std::string myName{};
	std::array<fs::path, Count> myPaths{};
	std::array<Image, Count> myImages{};
	std::array<TexturePtr, Count> myTextures{};
	std::array<ShaderResourcePtr, Count> myShaderResources{};
};

