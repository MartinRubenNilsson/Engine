#pragma once

class Cubemap
{
public:
	Cubemap() = default;
	Cubemap(std::span<const fs::path, 6> someLdrImages);
	Cubemap(const fs::path& anEquirectHdrImage);

	std::vector<ShaderResourcePtr> GetMaps() const; // { Environment, Irradiance, Prefiltered }

	explicit operator bool() const;

private:
	void CreateIrradianceMap();
	void CreatePrefilteredMap(); // todo : implement

	HRESULT myResult{ E_FAIL };
	ShaderResourcePtr myEnvironmentMap{}; // Used for skybox, reflections
	ShaderResourcePtr myIrradianceMap{};  // Used for diffuse IBL
	ShaderResourcePtr myPrefilteredMap{}; // Used for specular IBL
};

