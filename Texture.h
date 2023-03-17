#pragma once

enum class TextureType
{
	Unknown,
	Albedo,
	Normal,
	Metallic,
	Roughness,
	Occlusion,
	HDR,
};

const char* TextureTypeToString(TextureType); // Returns null-terminated name of texture
DXGI_FORMAT	TextureTypeToFormat(TextureType);
unsigned	TextureTypeToChannels(TextureType);

class Texture
{
public:
	using Ptr = std::shared_ptr<const Texture>;

	Texture() = default;
	Texture(const fs::path& aPath, TextureType aType);

	ShaderResourcePtr GetResource() const { return myShaderResource; }

	const fs::path& GetPath() const { return myPath; }
	TextureType GetType() const { return myType; }
	unsigned GetWidth() const { return myWidth; }
	unsigned GetHeight() const { return myHeight; }

	explicit operator bool() const;

private:
	friend void to_json(json&, const Texture::Ptr&);
	friend void from_json(const json&, Texture::Ptr&);

	fs::path myPath{};
	TexturePtr myTexture{};
	ShaderResourcePtr myShaderResource{};
	TextureType myType{};
	unsigned myWidth{}, myHeight{};
	HRESULT myResult{ E_FAIL };
};

using TextureFactory = AssetFactory<Texture>;

namespace ImGui
{
	//void Inspect(Texture::Ptr&); // todo
}

