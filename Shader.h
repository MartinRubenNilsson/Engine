#pragma once

enum class ShaderType
{
	Vertex,
	Pixel
};

class Shader
{
public:
	virtual ~Shader() = default;

	virtual void SetShader() const = 0;
	virtual void GetShader() = 0;
	virtual ShaderType GetType() const = 0;
	virtual operator bool() const = 0;
};

class VertexShader : public Shader
{
public:
	VertexShader() = default;
	VertexShader(std::string_view someBytecode);

	void SetShader() const override;
	void GetShader() override;
	ShaderType GetType() const override { return ShaderType::Vertex; }
	operator bool() const override { return myShader; }

private:
	ComPtr<ID3D11VertexShader> myShader{};
};

class PixelShader : public Shader
{
public:
	PixelShader() = default;
	PixelShader(std::string_view someBytecode);

	void SetShader() const override;
	void GetShader() override;
	ShaderType GetType() const override { return ShaderType::Pixel; }
	operator bool() const override { return myShader; }

private:
	ComPtr<ID3D11PixelShader> myShader{};
};

class ShaderManager : public Singleton<ShaderManager>
{
public:
	template <class ShaderType>
	std::shared_ptr<const ShaderType> GetShader(const fs::path& aPath);

private:
	std::unordered_map<fs::path, std::shared_ptr<Shader>> myShaders{};
};

template<class ShaderType>
inline std::shared_ptr<const ShaderType> ShaderManager::GetShader(const fs::path& aPath)
{
	auto itr = myShaders.find(aPath);
	if (itr != myShaders.end())
		return std::dynamic_pointer_cast<ShaderType>(itr->second);

	std::ifstream file{ aPath, std::ios::binary };
	if (!file)
		return nullptr;

	std::string bytecode = { std::istreambuf_iterator{ file }, {} };
	if (bytecode.empty())
		return nullptr;

	auto shader{ std::make_shared<ShaderType>(bytecode) };
	if (!shader->operator bool())
		return nullptr;

	myShaders.emplace(aPath, shader);
	return shader;
}

#define VERTEX_SHADER(aPath) ShaderManager::Get().GetShader<VertexShader>(aPath)
#define PIXEL_SHADER(aPath) ShaderManager::Get().GetShader<PixelShader>(aPath)

