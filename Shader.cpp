#include "pch.h"
#include "Shader.h"

/*
* class VertexShader
*/

VertexShader::VertexShader(const std::string& someBytecode)
	: Shader(someBytecode)
{
	DX11_DEVICE->CreateVertexShader(someBytecode.data(), someBytecode.size(), NULL, &myShader);
}

void VertexShader::SetShader() const
{
	DX11_CONTEXT->VSSetShader(myShader.Get(), NULL, 0);
}

/*
* class PixelShader
*/

PixelShader::PixelShader(const std::string& someBytecode)
	: Shader(someBytecode)
{
	DX11_DEVICE->CreatePixelShader(someBytecode.data(), someBytecode.size(), NULL, &myShader);
}

void PixelShader::SetShader() const
{
	DX11_CONTEXT->PSSetShader(myShader.Get(), NULL, 0);
}
