#pragma once

class Shader
{
public:
	virtual ~Shader() = default;

	virtual void SetShader() const = 0;
	virtual operator bool() const = 0;

	std::string_view GetBytecode() const { return myBytecode; }

protected:
	Shader(const std::string& someBytecode)
		: myBytecode{ someBytecode }
	{}

	std::string myBytecode;
};

class VertexShader : public Shader
{
public:
	VertexShader(const std::string& someBytecode);

	void SetShader() const override;
	operator bool() const override { return myShader; }

private:
	ComPtr<ID3D11VertexShader> myShader;
};

class PixelShader : public Shader
{
public:
	PixelShader(const std::string& someBytecode);

	void SetShader() const override;
	operator bool() const override { return myShader; }

private:
	ComPtr<ID3D11PixelShader> myShader;
};

class ShaderManager : public Singleton<ShaderManager>
{
public:
	template <class ShaderType>
	std::shared_ptr<ShaderType> GetShader(const fs::path& aPath);

private:
	std::unordered_map<fs::path, std::shared_ptr<Shader>> myShaders;
};

template<class ShaderType>
inline std::shared_ptr<ShaderType> ShaderManager::GetShader(const fs::path& aPath)
{
	auto itr = myShaders.find(aPath);
	if (itr != myShaders.end())
		return std::dynamic_pointer_cast<ShaderType>(itr->second);

	std::ifstream file{ aPath, std::ios::binary };
	if (!file)
		return nullptr;

	std::string bytecode = { std::istreambuf_iterator{ file }, {} };

	auto shader{ std::make_shared<ShaderType>(bytecode) };
	if (!*shader)
		return nullptr;

	myShaders.emplace(aPath, shader);
	return shader;
}

#define DX11_GET_VERTEX_SHADER(aPath) ShaderManager::Get().GetShader<VertexShader>(aPath)
#define DX11_GET_PIXEL_SHADER(aPath) ShaderManager::Get().GetShader<PixelShader>(aPath)