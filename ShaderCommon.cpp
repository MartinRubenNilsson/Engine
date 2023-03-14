#include "pch.h"
#include "ShaderCommon.h"
#include "FullscreenPass.h"

ImmutableBuffer ImmutableBuffer::Create()
{
	ImmutableBuffer buffer{};

	// cubeFaceViewProjs
	{
		const XMVECTOR origin{ 0.f, 0.f, 0.f, 1.f };
		const XMVECTOR x{ 1.f, 0.f, 0.f, 0.f };
		const XMVECTOR y{ 0.f, 1.f, 0.f, 0.f };
		const XMVECTOR z{ 0.f, 0.f, 1.f, 0.f };
		const XMMATRIX proj{ XMMatrixPerspectiveFovLH(XM_PIDIV2, 1.f, 0.1f, 10.f) };

		buffer.cubeFaceViewProjs =
		{
			XMMatrixLookToLH(origin, +x, +y) * proj,
			XMMatrixLookToLH(origin, -x, +y) * proj,
			XMMatrixLookToLH(origin, +y, -z) * proj,
			XMMatrixLookToLH(origin, -y, +z) * proj,
			XMMatrixLookToLH(origin, +z, +y) * proj,
			XMMatrixLookToLH(origin, -z, +y) * proj,
		};
	}

	// offsetVectors
	{
		buffer.offsetVectors =
		{
			// 8 pairwise opposite corners

			Vector4{ +1.f, +1.f, +1.f, 0.f },
			Vector4{ -1.f, -1.f, -1.f, 0.f },

			Vector4{ -1.f, +1.f, +1.f, 0.f },
			Vector4{ +1.f, -1.f, -1.f, 0.f },

			Vector4{ +1.f, +1.f, -1.f, 0.f },
			Vector4{ -1.f, -1.f, +1.f, 0.f },

			Vector4{ -1.f, +1.f, -1.f, 0.f },
			Vector4{ +1.f, -1.f, +1.f, 0.f },

			// 6 pairwise opposite face centers

			Vector4{ -1.f, 0.f, 0.f, 0.f },
			Vector4{ +1.f, 0.f, 0.f, 0.f },

			Vector4{ 0.f, -1.f, 0.f, 0.f },
			Vector4{ 0.f, +1.f, 0.f, 0.f },

			Vector4{ 0.f, 0.f, -1.f, 0.f },
			Vector4{ 0.f, 0.f, +1.f, 0.f },
		};

		std::default_random_engine engine{ 1337 };
		std::uniform_real_distribution<float> uniform{ 0.25f, 1.f };

		for (Vector4& v : buffer.offsetVectors)
		{
			v.Normalize();
			v *= uniform(engine);
		}
	}

	return buffer;
}

std::array<D3D11_SAMPLER_DESC, SamplerCount> GetSamplerDescs()
{
	std::array<D3D11_SAMPLER_DESC, SamplerCount> samplers{};
	samplers.fill(CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} });

	samplers[s_PointSampler].Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

	samplers[s_TrilinearSampler].Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	samplers[s_NormalDepthSampler].Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplers[s_NormalDepthSampler].AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplers[s_NormalDepthSampler].AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplers[s_NormalDepthSampler].BorderColor[0] = 0.f;
	samplers[s_NormalDepthSampler].BorderColor[1] = 0.f;
	samplers[s_NormalDepthSampler].BorderColor[2] = 0.f;
	samplers[s_NormalDepthSampler].BorderColor[3] = FAR_Z;

	samplers[s_GaussianSampler].Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplers[s_GaussianSampler].AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplers[s_GaussianSampler].AddressV = D3D11_TEXTURE_ADDRESS_WRAP;

	return samplers;
}

ShaderResourcePtr CreateGaussianMap()
{
	static constexpr unsigned size{ 512 };
	static constexpr unsigned seed{ 134534 };

	std::vector<float> samples(size * size * 4);

	{
		std::default_random_engine engine{ seed };
		std::normal_distribution<float> gaussian{};

		for (float& sample : samples)
			sample = gaussian(engine);
	}

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = size;
	desc.Height = size;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data{};
	data.pSysMem = samples.data();
	data.SysMemPitch = size * 4 * sizeof(float);
	data.SysMemSlicePitch = 0;

	TexturePtr texture{};

	if (FAILED(DX11_DEVICE->CreateTexture2D(&desc, &data, &texture)))
		return nullptr;

	ShaderResourcePtr resource{};

	if (FAILED(DX11_DEVICE->CreateShaderResourceView(texture.Get(), NULL, &resource)))
		return nullptr;

	return resource;
}

ShaderResourcePtr CreateIntegrationMap()
{
	static constexpr unsigned size{ 512 }; // todo: adjust size

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = size;
	desc.Height = size;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16G16_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	TexturePtr texture{};

	if (FAILED(DX11_DEVICE->CreateTexture2D(&desc, NULL, &texture)))
		return nullptr;

	RenderTargetPtr target{};

	if (FAILED(DX11_DEVICE->CreateRenderTargetView(texture.Get(), NULL, &target)))
		return nullptr;

	{
		ScopedTargets scopedTarget{ target };
		ScopedViewports scopedViewport{ CD3D11_VIEWPORT{ texture.Get(), target.Get() } };
		FullscreenPass{ "PsIntegrateBRDF.cso" }.Render();
	}

	ShaderResourcePtr resource{};

	if (FAILED(DX11_DEVICE->CreateShaderResourceView(texture.Get(), NULL, &resource)))
		return nullptr;

	return resource;
}
