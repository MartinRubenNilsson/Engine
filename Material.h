#pragma once
#include "Image.h"

enum class TextureType : size_t
{
	Diffuse,
	Emissive,
	Normal,
	Metallic,
	Roughness,
	Count
};

const char* TextureTypeToString(TextureType);

class Material
{
public:
	Material(const aiMaterial&);

	std::string_view GetName() const { return myName; }

	const fs::path& GetPath(TextureType) const;
	const Image& GetImage(TextureType) const;
	TexturePtr GetTexture(TextureType) const;
	ShaderResourcePtr GetShaderResource(TextureType) const;

	operator std::span<const ShaderResourcePtr>() const { return myShaderResources; }

private:
	static constexpr size_t ourCount = std::to_underlying(TextureType::Count);

	static constexpr std::array<unsigned, ourCount> ourChannels
	{
		4, // Diffuse
		4, // Emissive
		4, // Normal
		1, // Metallic
		1, // Roughness
	};

	static constexpr std::array<DXGI_FORMAT, ourCount> ourFormats
	{
		DXGI_FORMAT_R8G8B8A8_UNORM, // Diffuse
		DXGI_FORMAT_R8G8B8A8_UNORM, // Emissive
		DXGI_FORMAT_R8G8B8A8_UNORM, // Normal
		DXGI_FORMAT_R8_UNORM,		// Metallic
		DXGI_FORMAT_R8_UNORM,		// Roughness
	};

	void LoadPaths(const aiMaterial&);
	void LoadImages();
	void CreateTextures();
	void CreateShaderResources();

	std::string myName{};
	std::array<fs::path, ourCount> myPaths{};
	std::array<Image, ourCount> myImages{};
	std::array<TexturePtr, ourCount> myTextures{};
	std::array<ShaderResourcePtr, ourCount> myShaderResources{};
};

namespace ImGui
{
	void InspectMaterial(const Material&);
}