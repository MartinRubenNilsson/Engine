#pragma once

class Cubemap
{
public:
	Cubemap() = default;
	Cubemap(std::span<const fs::path, 6> someLdrImages);
	//Cubemap(const fs::path& anEquirectHdrImage);

	void DrawSkybox() const;

	ShaderResourcePtr GetEnvironmentMap() const { return myEnvironmentMap; }
	ShaderResourcePtr GetIrradianceMap() const { return myIrradianceMap; }

	explicit operator bool() const;

private:
	void CreateIrradianceMap();

	HRESULT myResult{ E_FAIL };
	ShaderResourcePtr myEnvironmentMap{};
	ShaderResourcePtr myIrradianceMap{};
};

