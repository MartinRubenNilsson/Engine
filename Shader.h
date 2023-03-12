#pragma once

enum class ShaderType
{
	Pixel	 = D3D11_SHVER_PIXEL_SHADER,
	Vertex	 = D3D11_SHVER_VERTEX_SHADER,
	Geometry = D3D11_SHVER_GEOMETRY_SHADER,
	Hull	 = D3D11_SHVER_HULL_SHADER,
	Domain	 = D3D11_SHVER_DOMAIN_SHADER,
	Compute	 = D3D11_SHVER_COMPUTE_SHADER,
};

using ShaderVariant = std::variant
<
	PixelShaderPtr, VertexShaderPtr, GeometryShaderPtr,
	HullShaderPtr, DomainShaderPtr, ComputeShaderPtr
>;

void EmplaceShader(ShaderType, ShaderVariant&);
void SetShader(const ShaderVariant&);
void GetShader(ShaderVariant&);

class Shader
{
public:
	Shader() = default;
	Shader(const fs::path&);

	void SetShader() const;

	ShaderType GetType() const;
	const fs::path& GetPath() const { return myPath; }
	std::string_view GetBytecode() const { return myBytecode; }

	operator bool() const noexcept;

private:
	HRESULT myResult{ E_FAIL };
	ShaderVariant myShader{};
	fs::path myPath{};
	std::string myBytecode{};
};

using ShaderFactory = AssetFactory<Shader>;

