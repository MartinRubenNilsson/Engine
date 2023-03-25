#pragma once

enum class TextureType
{
	Unknown,
	Albedo,
	Normal,
	Metallic,
	Roughness,
	Occlusion,
	Cubemap,
};

const char* ToString(TextureType);
DXGI_FORMAT	ToFormat(TextureType);
unsigned	ToChannels(TextureType);

std::span<const TextureType> GetMaterialTextureTypes();

class Texture
{
public:
	using Ptr = std::shared_ptr<const Texture>;

	Texture() = default;
	Texture(const fs::path&, TextureType aType = TextureType::Unknown);

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
	TextureType myType{ TextureType::Unknown };
	unsigned myWidth{}, myHeight{};
	HRESULT myResult{ E_FAIL };
};

using TextureFactory = AssetFactory<Texture>;

