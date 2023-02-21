#pragma once
#include "Image.h"

enum class TextureType : size_t
{
	Albedo,
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
	using Ptr = std::shared_ptr<const Material>;

	Material(const aiMaterial&);

	std::string_view GetName() const { return myName; }

	const fs::path& GetPath(TextureType) const;
	ShaderResourcePtr GetShaderResource(TextureType) const;

	operator std::span<const ShaderResourcePtr>() const { return myShaderResources; }

private:
	static constexpr size_t ourCount = std::to_underlying(TextureType::Count);

	static constexpr std::array<unsigned, ourCount> ourChannels
	{
		4, // Albedo
		4, // Normal
		1, // Metallic
		1, // Roughness
		1, // Occlusion
		//1, // Emissive
	};

	static constexpr std::array<DXGI_FORMAT, ourCount> ourFormats
	{
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, // Albedo
		DXGI_FORMAT_R8G8B8A8_UNORM,		 // Normal
		DXGI_FORMAT_R8_UNORM,			 // Metallic
		DXGI_FORMAT_R8_UNORM,			 // Roughness
		DXGI_FORMAT_R8_UNORM,			 // Occlusion
		//DXGI_FORMAT_R8_UNORM,			 // Emissive
	};

	void LoadAndCreateTexturesAndResources();

	std::string myName{};
	std::array<fs::path, ourCount> myTexturePaths{};
	std::array<TexturePtr, ourCount> myTextures{};
	std::array<ShaderResourcePtr, ourCount> myShaderResources{};
};

namespace ImGui
{
	void InspectMaterial(const Material&);
}

