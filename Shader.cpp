#include "pch.h"
#include "Shader.h"

namespace
{
	struct Creator
	{
		std::string_view bytecode{};

		HRESULT operator()(PixelShaderPtr& ptr) const { return DX11_DEVICE->CreatePixelShader(bytecode.data(), bytecode.size(), NULL, &ptr); }
		HRESULT operator()(VertexShaderPtr& ptr) const { return DX11_DEVICE->CreateVertexShader(bytecode.data(), bytecode.size(), NULL, &ptr); }
		HRESULT operator()(GeometryShaderPtr& ptr) const { return DX11_DEVICE->CreateGeometryShader(bytecode.data(), bytecode.size(), NULL, &ptr); }
		HRESULT operator()(HullShaderPtr& ptr)	const { return DX11_DEVICE->CreateHullShader(bytecode.data(), bytecode.size(), NULL, &ptr); }
		HRESULT operator()(DomainShaderPtr& ptr) const { return DX11_DEVICE->CreateDomainShader(bytecode.data(), bytecode.size(), NULL, &ptr); }
		HRESULT operator()(ComputeShaderPtr& ptr) const { return DX11_DEVICE->CreateComputeShader(bytecode.data(), bytecode.size(), NULL, &ptr); }
	};

	struct Setter
	{
		void operator()(const PixelShaderPtr& ptr) const { DX11_CONTEXT->PSSetShader(ptr.Get(), NULL, 0); }
		void operator()(const VertexShaderPtr& ptr) const { DX11_CONTEXT->VSSetShader(ptr.Get(), NULL, 0); }
		void operator()(const GeometryShaderPtr& ptr) const { DX11_CONTEXT->GSSetShader(ptr.Get(), NULL, 0); }
		void operator()(const HullShaderPtr& ptr) const { DX11_CONTEXT->HSSetShader(ptr.Get(), NULL, 0); }
		void operator()(const DomainShaderPtr& ptr) const { DX11_CONTEXT->DSSetShader(ptr.Get(), NULL, 0); }
		void operator()(const ComputeShaderPtr& ptr) const { DX11_CONTEXT->CSSetShader(ptr.Get(), NULL, 0); }
	};

	struct Getter
	{
		void operator()(PixelShaderPtr& ptr) const { DX11_CONTEXT->PSGetShader(&ptr, NULL, 0); }
		void operator()(VertexShaderPtr& ptr) const { DX11_CONTEXT->VSGetShader(&ptr, NULL, 0); }
		void operator()(GeometryShaderPtr& ptr) const { DX11_CONTEXT->GSGetShader(&ptr, NULL, 0); }
		void operator()(HullShaderPtr& ptr) const { DX11_CONTEXT->HSGetShader(&ptr, NULL, 0); }
		void operator()(DomainShaderPtr& ptr) const { DX11_CONTEXT->DSGetShader(&ptr, NULL, 0); }
		void operator()(ComputeShaderPtr& ptr) const { DX11_CONTEXT->CSGetShader(&ptr, NULL, 0); }
	};
}

/*
* class Shader
*/

Shader::Shader(std::string_view someBytecode)
{
	ComPtr<ID3D11ShaderReflection> reflector{};
	myResult = D3DReflect(someBytecode.data(), someBytecode.size(), IID_PPV_ARGS(&reflector));
	if (FAILED(myResult))
		return;

	D3D11_SHADER_DESC desc{};
	myResult = reflector->GetDesc(&desc);
	if (FAILED(myResult))
		return;

	switch (D3D11_SHVER_GET_TYPE(desc.Version))
	{
	case D3D11_SHVER_PIXEL_SHADER:
		myShader.emplace<D3D11_SHVER_PIXEL_SHADER>();
		break;
	case D3D11_SHVER_VERTEX_SHADER:
		myShader.emplace<D3D11_SHVER_VERTEX_SHADER>();
		break;
	case D3D11_SHVER_GEOMETRY_SHADER:
		myShader.emplace<D3D11_SHVER_GEOMETRY_SHADER>();
		break;
	case D3D11_SHVER_HULL_SHADER:
		myShader.emplace<D3D11_SHVER_HULL_SHADER>();
		break;
	case D3D11_SHVER_DOMAIN_SHADER:
		myShader.emplace<D3D11_SHVER_DOMAIN_SHADER>();
		break;
	case D3D11_SHVER_COMPUTE_SHADER:
		myShader.emplace<D3D11_SHVER_COMPUTE_SHADER>();
		break;
	}

	myResult = std::visit(Creator{ someBytecode }, myShader);
}

void Shader::SetShader() const
{
	if (operator bool())
		std::visit(Setter{}, myShader);
}

void Shader::GetShader()
{
	if (operator bool())
		std::visit(Getter{}, myShader);
}

ShaderType Shader::GetType() const
{
	return static_cast<ShaderType>(myShader.index());
}

Shader::operator bool() const noexcept
{
	return SUCCEEDED(myResult);
}

/*
* class ShaderFactory
*/

std::shared_ptr<const Shader> ShaderFactory::GetShader(const fs::path& aPath)
{
	auto itr = myShaders.find(aPath);
	if (itr != myShaders.end())
		return itr->second;

	std::ifstream file{ "shaders" / aPath, std::ios::binary};
	if (!file)
		return nullptr;

	std::string bytecode{ std::istreambuf_iterator{ file }, {} };
	if (bytecode.empty())
		return nullptr;

	auto shader{ std::make_shared<Shader>(bytecode) };
	if (!*shader)
		return nullptr;

	myShaders.emplace(aPath, shader);
	return shader;
}
