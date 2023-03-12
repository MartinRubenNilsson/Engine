#pragma once

class Cubemap
{
public:
	Cubemap() = default;
	Cubemap(const fs::path&); // Path to equirectangular HDR environment map

	const auto& GetMaps() const { return myMaps; }
	const fs::path GetPath() const { return myPath; }

	explicit operator bool() const;

private:
	enum MapType
	{
		Environment, // Used for skybox
		Irradiance,	 // Used for diffuse IBL
		Prefiltered, // Used for specular IBL
		Count
	};

	void CreateEnvironmentMap(ShaderResourcePtr anEquirectMap);
	void CreateIrradianceMap();
	void CreatePrefilteredMap();

	HRESULT myResult{ E_FAIL };
	fs::path myPath{};
	std::array<ShaderResourcePtr, Count> myMaps{};
};

