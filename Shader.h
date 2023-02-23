#pragma once

enum class ShaderType
{
	Vertex,
	Geometry,
	Pixel,
};

class Shader
{
public:
	virtual ~Shader() = default;

	virtual void SetShader() const = 0;
	virtual void GetShader() = 0;
	virtual ShaderType GetType() const = 0;
	virtual explicit operator bool() const = 0;
};

class VertexShader : public Shader
{
public:
	VertexShader() = default;
	VertexShader(std::string_view someBytecode);

	void SetShader() const override;
	void GetShader() override;
	ShaderType GetType() const override { return ShaderType::Vertex; }
	explicit operator bool() const override { return myShader; }

private:
	ComPtr<ID3D11VertexShader> myShader{};
};

class GeometryShader : public Shader
{
public:
	GeometryShader() = default;
	GeometryShader(std::string_view someBytecode);

	void SetShader() const override;
	void GetShader() override;
	ShaderType GetType() const override { return ShaderType::Geometry; }
	explicit operator bool() const override { return myShader; }

private:
	ComPtr<ID3D11GeometryShader> myShader{};
};

class PixelShader : public Shader
{
public:
	PixelShader() = default;
	PixelShader(std::string_view someBytecode);

	void SetShader() const override;
	void GetShader() override;
	ShaderType GetType() const override { return ShaderType::Pixel; }
	explicit operator bool() const override { return myShader; }

private:
	ComPtr<ID3D11PixelShader> myShader{};
};

class ShaderFactory : public Singleton<ShaderFactory>
{
public:
	template <class T>
	std::shared_ptr<const T> GetShader(const fs::path& aPath);

private:
	std::unordered_map<fs::path, std::shared_ptr<Shader>> myShaders{};
};

template<class T>
inline std::shared_ptr<const T> ShaderFactory::GetShader(const fs::path& aPath)
{
	static_assert(std::is_base_of_v<Shader, T>);

	auto itr = myShaders.find(aPath);
	if (itr != myShaders.end())
		return std::dynamic_pointer_cast<const T>(itr->second);

	std::ifstream file{ aPath, std::ios::binary };
	if (!file)
		return nullptr;

	std::string bytecode = { std::istreambuf_iterator{ file }, {} };
	if (bytecode.empty())
		return nullptr;

	auto shader{ std::make_shared<T>(bytecode) };
	if (!shader->operator bool())
		return nullptr;

	myShaders.emplace(aPath, shader);
	return shader;
}

#define VERTEX_SHADER(aPath) ShaderFactory::Get().GetShader<VertexShader>(aPath)
#define GEOMETRY_SHADER(aPath) ShaderFactory::Get().GetShader<GeometryShader>(aPath)
#define PIXEL_SHADER(aPath) ShaderFactory::Get().GetShader<PixelShader>(aPath)

