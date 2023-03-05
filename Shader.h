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
	Shader(std::string_view someBytecode);

	void SetShader() const;
	void GetShader();

	ShaderType GetType() const;

	operator bool() const noexcept;

private:
	HRESULT myResult{ E_FAIL };
	std::variant<
		PixelShaderPtr,
		VertexShaderPtr,
		GeometryShaderPtr,
		HullShaderPtr,
		DomainShaderPtr,
		ComputeShaderPtr
	> myShader{};
};

class ShaderFactory : public Singleton<ShaderFactory>
{
public:
	std::shared_ptr<const Shader> GetShader(const fs::path& aPath);

private:
	std::unordered_map<fs::path, std::shared_ptr<Shader>> myShaders{};
};

