#include "pch.h"
#include "Shader.h"

/*
* class VertexShader
*/

VertexShader::VertexShader(std::string_view someBytecode)
{
	DX11_DEVICE->CreateVertexShader(someBytecode.data(), someBytecode.size(), NULL, &myShader);
}

void VertexShader::SetShader() const
{
	DX11_CONTEXT->VSSetShader(myShader.Get(), NULL, 0);
}

void VertexShader::GetShader()
{
	DX11_CONTEXT->VSGetShader(&myShader, NULL, NULL);
}

/*
* class GeometryShader
*/

GeometryShader::GeometryShader(std::string_view someBytecode)
{
	DX11_DEVICE->CreateGeometryShader(someBytecode.data(), someBytecode.size(), NULL, &myShader);
}

void GeometryShader::SetShader() const
{
	DX11_CONTEXT->GSSetShader(myShader.Get(), NULL, 0);
}

void GeometryShader::GetShader()
{
	DX11_CONTEXT->GSGetShader(&myShader, NULL, NULL);
}

/*
* class PixelShader
*/

PixelShader::PixelShader(std::string_view someBytecode)
{
	DX11_DEVICE->CreatePixelShader(someBytecode.data(), someBytecode.size(), NULL, &myShader);
}

void PixelShader::SetShader() const
{
	DX11_CONTEXT->PSSetShader(myShader.Get(), NULL, 0);
}

void PixelShader::GetShader()
{
	DX11_CONTEXT->PSGetShader(&myShader, NULL, NULL);
}
