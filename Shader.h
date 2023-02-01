#pragma once

class Shader
{
public:
	virtual ~Shader() = default;

	virtual void SetShader() const = 0;
	virtual operator bool() const = 0;

	const fs::path& GetPath() const { return myPath; }
	const std::string& GetBytecode() const { return myBytecode; }

protected:
	Shader(const fs::path&);

	fs::path myPath;
	std::string myBytecode;
};

class VertexShader : public Shader
{
public:
	VertexShader(const fs::path&);

	void SetShader() const override;
	operator bool() const override { return myShader; }

private:
	ComPtr<ID3D11VertexShader> myShader;
};

class PixelShader : public Shader
{
public:
	PixelShader(const fs::path&);

	void SetShader() const override;
	operator bool() const override { return myShader; }

private:
	ComPtr<ID3D11PixelShader> myShader;
};

