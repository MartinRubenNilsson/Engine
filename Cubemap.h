#pragma once

class Cubemap
{
public:
	Cubemap() = default;
	Cubemap(std::span<const fs::path, 6> someLdrImages);
	//Cubemap(const fs::path& anEquirectHdrImage);

	// Returns environment/irradiance/prefiltered maps in that order.
	std::vector<ShaderResourcePtr> GetMaps() const;

	explicit operator bool() const;

private:
	void CreateIrradianceMap();

	HRESULT myResult{ E_FAIL };
	ShaderResourcePtr myEnvironmentMap{}; // Skybox
	ShaderResourcePtr myIrradianceMap{};  // Diffuse IBL
	ShaderResourcePtr myPrefilteredMap{}; // Specular IBL
};

