#include "pch.h"
#include "Shader.h"
#include "DX11.h"

void EmplaceShader(ShaderType aType, ShaderVariant& aVariant)
{
	switch (aType)
	{
	case ShaderType::Pixel:
		aVariant.emplace<PixelShaderPtr>();
		break;
	case ShaderType::Vertex:
		aVariant.emplace<VertexShaderPtr>();
		break;
	case ShaderType::Geometry:
		aVariant.emplace<GeometryShaderPtr>();
		break;
	case ShaderType::Hull:
		aVariant.emplace<HullShaderPtr>();
		break;
	case ShaderType::Domain:
		aVariant.emplace<DomainShaderPtr>();
		break;
	case ShaderType::Compute:
		aVariant.emplace<ComputeShaderPtr>();
		break;
	}
}

void SetShader(const ShaderVariant& aVariant)
{
	struct Setter
	{
		void operator()(const PixelShaderPtr& ptr) const { DX11::GetContext()->PSSetShader(ptr.Get(), NULL, 0); }
		void operator()(const VertexShaderPtr& ptr) const { DX11::GetContext()->VSSetShader(ptr.Get(), NULL, 0); }
		void operator()(const GeometryShaderPtr& ptr) const { DX11::GetContext()->GSSetShader(ptr.Get(), NULL, 0); }
		void operator()(const HullShaderPtr& ptr) const { DX11::GetContext()->HSSetShader(ptr.Get(), NULL, 0); }
		void operator()(const DomainShaderPtr& ptr) const { DX11::GetContext()->DSSetShader(ptr.Get(), NULL, 0); }
		void operator()(const ComputeShaderPtr& ptr) const { DX11::GetContext()->CSSetShader(ptr.Get(), NULL, 0); }
	};

	std::visit(Setter{}, aVariant);
}

void GetShader(ShaderVariant& aVariant)
{
	struct Getter
	{
		void operator()(PixelShaderPtr& ptr) const { DX11::GetContext()->PSGetShader(&ptr, NULL, 0); }
		void operator()(VertexShaderPtr& ptr) const { DX11::GetContext()->VSGetShader(&ptr, NULL, 0); }
		void operator()(GeometryShaderPtr& ptr) const { DX11::GetContext()->GSGetShader(&ptr, NULL, 0); }
		void operator()(HullShaderPtr& ptr) const { DX11::GetContext()->HSGetShader(&ptr, NULL, 0); }
		void operator()(DomainShaderPtr& ptr) const { DX11::GetContext()->DSGetShader(&ptr, NULL, 0); }
		void operator()(ComputeShaderPtr& ptr) const { DX11::GetContext()->CSGetShader(&ptr, NULL, 0); }
	};

	std::visit(Getter{}, aVariant);
}

/*
* class Shader
*/

Shader::Shader(const fs::path& aPath)
{
	std::ifstream file{ "shaders" / aPath, std::ios::binary };
	if (!file)
		return;

	std::string bytecode{ std::istreambuf_iterator{ file }, {} };
	if (bytecode.empty())
		return;

	ComPtr<ID3D11ShaderReflection> reflector{};
	myResult = D3DReflect(bytecode.data(), bytecode.size(), IID_PPV_ARGS(&reflector));
	if (FAILED(myResult))
		return;

	D3D11_SHADER_DESC desc{};
	myResult = reflector->GetDesc(&desc);
	if (FAILED(myResult))
		return;

	ShaderType type{ D3D11_SHVER_GET_TYPE(desc.Version) };
	EmplaceShader(type, myShader);

	struct Creator
	{
		std::string_view bytecode{};

		HRESULT operator()(PixelShaderPtr& ptr) const { return DX11::GetDevice()->CreatePixelShader(bytecode.data(), bytecode.size(), NULL, &ptr); }
		HRESULT operator()(VertexShaderPtr& ptr) const { return DX11::GetDevice()->CreateVertexShader(bytecode.data(), bytecode.size(), NULL, &ptr); }
		HRESULT operator()(GeometryShaderPtr& ptr) const { return DX11::GetDevice()->CreateGeometryShader(bytecode.data(), bytecode.size(), NULL, &ptr); }
		HRESULT operator()(HullShaderPtr& ptr)	const { return DX11::GetDevice()->CreateHullShader(bytecode.data(), bytecode.size(), NULL, &ptr); }
		HRESULT operator()(DomainShaderPtr& ptr) const { return DX11::GetDevice()->CreateDomainShader(bytecode.data(), bytecode.size(), NULL, &ptr); }
		HRESULT operator()(ComputeShaderPtr& ptr) const { return DX11::GetDevice()->CreateComputeShader(bytecode.data(), bytecode.size(), NULL, &ptr); }
	};

	myResult = std::visit(Creator{ bytecode }, myShader);
	if (FAILED(myResult))
		return;

	myPath = aPath;
	myBytecode = std::move(bytecode);
}

void Shader::SetShader() const
{
	if (operator bool())
		::SetShader(myShader);
}

ShaderType Shader::GetType() const
{
	return static_cast<ShaderType>(myShader.index());
}

Shader::operator bool() const noexcept
{
	return SUCCEEDED(myResult);
}
