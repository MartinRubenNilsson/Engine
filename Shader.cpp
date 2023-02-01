#include "pch.h"
#include "Shader.h"

Shader::Shader(const fs::path& aPath)
	: myPath{ aPath }
	, myBytecode{}
{
	std::ifstream file{ aPath, std::ios::binary };
	myBytecode = { std::istreambuf_iterator{ file }, {} };
}

VertexShader::VertexShader(const fs::path& aPath)
	: Shader(aPath)
{
	DX11_DEVICE->CreateVertexShader(myBytecode.data(), myBytecode.size(), NULL, &myShader);
}

void VertexShader::SetShader() const
{
	DX11_CONTEXT->VSSetShader(myShader.Get(), NULL, 0);
}

PixelShader::PixelShader(const fs::path& aPath)
	: Shader(aPath)
{
	DX11_DEVICE->CreatePixelShader(myBytecode.data(), myBytecode.size(), NULL, &myShader);
}

void PixelShader::SetShader() const
{
	DX11_CONTEXT->PSSetShader(myShader.Get(), NULL, 0);
}

