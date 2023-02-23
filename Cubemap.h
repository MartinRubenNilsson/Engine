#pragma once

class Cubemap
{
public:
	Cubemap() = default;
	Cubemap(std::span<const fs::path, 6> someLdrImages);
	Cubemap(const fs::path& anEquirectHdrImage);

	void DrawSkybox() const;

	ShaderResourcePtr GetResource() const { return myShaderResource; }

	explicit operator bool() const;

private:
	HRESULT myResult{ E_FAIL };
	ShaderResourcePtr myShaderResource{};
};

