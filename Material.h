#pragma once
#include "Image.h"

enum class TextureType : size_t
{
	BaseColor,
	Normal,
	Metallic,
	Roughness,
	Occlusion,
	//Emissive,
	Count
};

const char* TextureTypeToString(TextureType);

class Material
{
public:
	Material(const aiMaterial&);

	std::string_view GetName() const { return myName; }

	const fs::path& GetPath(TextureType) const;
	ShaderResourcePtr GetShaderResource(TextureType) const;

	operator std::span<const ShaderResourcePtr>() const { return myShaderResources; }

private:
	static constexpr size_t ourCount = std::to_underlying(TextureType::Count);

	static constexpr std::array<unsigned, ourCount> ourChannels
	{
		4, // BaseColor
		4, // Normal
		1, // Metallic
		1, // Roughness
		1, // Occlusion
		//1, // Emissive
	};

	static constexpr std::array<DXGI_FORMAT, ourCount> ourFormats
	{
		DXGI_FORMAT_R8G8B8A8_UNORM, // BaseColor
		DXGI_FORMAT_R8G8B8A8_UNORM, // Normal
		DXGI_FORMAT_R8_UNORM,		// Metallic
		DXGI_FORMAT_R8_UNORM,		// Roughness
		DXGI_FORMAT_R8_UNORM,		// Occlusion
		//DXGI_FORMAT_R8_UNORM,		// Emissive
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