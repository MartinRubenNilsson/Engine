#include "pch.h"
#include "Cubemap.h"
#include "Scopes.h"
#include "ShaderCommon.h"
#include "Texture.h"
#include "ConstantBuffer.h"

Cubemap::Cubemap(const fs::path& aPath)
	: myPath{ aPath }
{
	auto equirectMap = TextureFactory::Get().GetAsset(aPath, TextureType::Cubemap);
	if (!equirectMap)
		return;

	ScopedTopology scopedTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };
	ScopedLayout scopedLayout{ typeid(EmptyVertex) };
	ScopedShader scopedVs{ "VsCubemap.cso" };
	ScopedShader scopedGs{ "GsGenCubemap.cso" };

	CreateEnvironmentMap(equirectMap->GetResource());
	if (FAILED(myResult))
		return;

	ScopedResources scopedResources{ ShaderType::Pixel, t_EnvironmentMap, { myMaps.at(Environment) } };
	
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

	ScopedShader scopedPs{ "PsEquirectToCubemap.cso" };
	ScopedResources scopedResource{ ShaderType::Pixel, 0, { anEquirectMap } };
	ScopedTargets scopedTarget{ target };
	ScopedViewports scopedViewport{ CD3D11_VIEWPORT{ texture.Get(), target.Get() } };

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

	ScopedShader scopedPs{ "PsGenIrradianceMap.cso" };
	ScopedTargets scopedTargets{ target };
	ScopedViewports scopedViewport{ CD3D11_VIEWPORT{ texture.Get(), target.Get() } };

	DX11_CONTEXT->Draw(CUBEMAP_VERTEX_COUNT, 0);
}

void Cubemap::CreatePrefilteredMap()
{
	static constexpr UINT size{ 128 };
	static constexpr UINT mipLevels{ 5 };

	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = size;
	textureDesc.Height = size;
	textureDesc.MipLevels = mipLevels;
	textureDesc.ArraySize = 6;
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	TexturePtr texture{};

	myResult = DX11_DEVICE->CreateTexture2D(&textureDesc, NULL, &texture);
	if (FAILED(myResult))
		return;

	myResult = DX11_DEVICE->CreateShaderResourceView(texture.Get(), NULL, &myMaps.at(Prefiltered));
	if (FAILED(myResult))
		return;

	struct PrefilterBuffer
	{
		Vector4 roughness{}; // (roughness, [unused], [unused], [unused])
	};

	ConstantBuffer cbuffer{ sizeof(PrefilterBuffer) };
	if (!cbuffer)
		return;

	cbuffer.PSSetBuffer(10);

	ScopedShader scopedPs{ "PsGenPrefilteredMap.cso" };

	for (UINT mip = 0; mip < mipLevels; ++mip)
	{
		PrefilterBuffer buffer{};
		buffer.roughness.x = static_cast<float>(mip) / (mipLevels - 1u);
		cbuffer.Update(&buffer);

		D3D11_RENDER_TARGET_VIEW_DESC targetDesc{};
		targetDesc.Format = textureDesc.Format;
		targetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		targetDesc.Texture2DArray.MipSlice = mip;
		targetDesc.Texture2DArray.FirstArraySlice = 0;
		targetDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;

		RenderTargetPtr target{};

		myResult = DX11_DEVICE->CreateRenderTargetView(texture.Get(), &targetDesc, &target);
		if (FAILED(myResult))
			return;

		ScopedTargets scopedTargets{ target };
		ScopedViewports scopedViewports{ CD3D11_VIEWPORT{ texture.Get(), target.Get() } };

		DX11_CONTEXT->Draw(CUBEMAP_VERTEX_COUNT, 0);
	}
}
