#pragma once

// Values match D3D11_SHVER_PIXEL_SHADER - do not reorder!
enum class ShaderType
{
	Pixel,
	Vertex,
	Geometry,
	Hull,
	Domain,
	Compute,
};

class Shader
{
public:
	Shader() = default;
	Shader(const fs::path&);

	void SetShader() const;
	void GetShader();

	ShaderType GetType() const;

	const fs::path& GetPath() const { return myPath; }
	std::string_view GetBytecode() const { return myBytecode; }

	operator bool() const noexcept;

private:
	using ShaderVariant = std::variant
	<
		PixelShaderPtr, VertexShaderPtr, GeometryShaderPtr,
		HullShaderPtr, DomainShaderPtr, ComputeShaderPtr
	>;

	HRESULT myResult{ E_FAIL };
	ShaderVariant myShader{};
	fs::path myPath{};
	std::string myBytecode{};
};

using ShaderFactory = AssetFactory<Shader>;

