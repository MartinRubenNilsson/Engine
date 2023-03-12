#include "pch.h"
#include "Cubemap.h"
#include "Scopes.h"
#include "ShaderCommon.h"
#include "Texture.h"

Cubemap::Cubemap(const fs::path& aPath)
	: myPath{ aPath }
{
	Texture::Ptr equirectMap{ TextureFactory::Get().GetAsset(aPath, TextureType::HDR) };
	if (!equirectMap)
		return;

	CreateEnvironmentMap(equirectMap->GetResource());
	if (FAILED(myResult))
		return;
	
	CreateIrradianceMap();
	CreatePrefilteredMap();
}

Cubemap::operator bool() const
{
	return SUCCEEDED(myResult);
}

void Cubemap::CreateEnvironmentMap(ShaderResourcePtr anEquirectMap)
{
	static constexpr UINT size{ 512 };

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = size;
	desc.Height = size;
	desc.MipLevels = 1;
	desc.ArraySize = 6;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	TexturePtr texture{};
	RenderTargetPtr target{};

	myResult = DX11_DEVICE->CreateTexture2D(&desc, NULL, &texture);
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateShaderResourceView(texture.Get(), NULL, &myMaps.at(Environment));
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateRenderTargetView(texture.Get(), NULL, &target);
	if (FAILED(myResult))
		return;

	ScopedPrimitiveTopology scopedTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };
	ScopedInputLayout scopedLayout{ typeid(EmptyVertex) };
	ScopedShader scopedVs{ "VsCubemap.cso" };
	ScopedShader scopedGs{ "GsGenCubemap.cso" };
	ScopedShader scopedPs{ "PsEquirectToCubemap.cso" };
	ScopedShaderResources scopedResource{ ShaderType::Pixel, 0, anEquirectMap };
	ScopedRenderTargets scopedTarget{ target };
	ScopedViewports scopedViewports{ CD3D11_VIEWPORT{ texture.Get(), target.Get() } };

	DX11_CONTEXT->Draw(CUBEMAP_VERTEX_COUNT, 0);
}

void Cubemap::CreateIrradianceMap()
{
	static constexpr UINT size{ 32 };

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = size;
	desc.Height = size;
	desc.MipLevels = 1;
	desc.ArraySize = 6;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	TexturePtr texture{};
	RenderTargetPtr target{};

	myResult = DX11_DEVICE->CreateTexture2D(&desc, NULL, &texture);
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateShaderResourceView(texture.Get(), NULL, &myMaps.at(Irradiance));
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateRenderTargetView(texture.Get(), NULL, &target);
	if (FAILED(myResult))
		return;

	ScopedPrimitiveTopology scopedTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };
	ScopedInputLayout scopedLayout{ typeid(EmptyVertex) };
	ScopedShader scopedVs{ "VsCubemap.cso" };
	ScopedShader scopedGs{ "GsGenCubemap.cso" };
	ScopedShader scopedPs{ "PsGenIrradianceMap.cso" };
	ScopedShaderResources scopedResources{ ShaderType::Pixel, t_EnvironmentMap, myMaps.at(Environment) };
	ScopedRenderTargets scopedTargets{ target };
	ScopedViewports scopedViewports{ CD3D11_VIEWPORT{ texture.Get(), target.Get() } };

	DX11_CONTEXT->Draw(CUBEMAP_VERTEX_COUNT, 0);
}

void Cubemap::CreatePrefilteredMap()
{
	 // todo
}
