#pragma once

#define FULLSCREEN_VERTEX_COUNT 3
#define CUBEMAP_VERTEX_COUNT 14

#define USE_REVERSE_Z 0

#if USE_REVERSE_Z
#define NEAR_Z 1.f
#define FAR_Z 0.f
#else
#define NEAR_Z 0.f
#define FAR_Z 1.f
#endif

/*
* Constant buffers
*/

struct alignas(16) ImmutableBuffer
{
	std::array<Matrix, 6> cubeFaceViewProjs; // Used when rendering to cubemaps
	std::array<Vector4, 14> offsetVectors; // Used when sampling over the hemisphere in SSAO

	static ImmutableBuffer Create();
};

struct alignas(16) CameraBuffer
{
	Matrix proj{};
	Matrix invProj{};
	Matrix viewProj{};
	Matrix invViewProj{};
	Vector4 position{};
	Vector4 clipPlanes{}; // (nearZ, farZ, [unused], [unused])
};

struct alignas(16) MeshBuffer
{
	Matrix matrix{};
	Matrix matrixInvTrans{};
	uint32_t entity[4]{};
};

struct alignas(16) LightBuffer
{
	Vector4 color{};	  // (r, g, b, [unused])
	Vector4 position{};   // (x, y, z, 1)
	Vector4 direction{};  // (x, y, z, 0)
	Vector4 parameters{}; // (range, constant attn, linear attn, quadratic attn)
	Vector4 coneAngles{}; // (inner, outer, [unused], [unused])
};

enum CBufferSlot : unsigned
{
	b_Immutable,
	b_Camera,
	b_Mesh,
	b_Light,
};

/*
* Textures
*/

enum TextureSlot : unsigned
{
	t_GBufferNormalDepth = 0,
	t_GBufferAlbedo,
	t_GBufferMetalRoughAo,
	t_GBufferEntity,

	t_AmbientAccessMap,
	t_BlurInputTexture,
	t_LightingTexture,

	t_MaterialAlbedo = 10,
	t_MaterialNormal,
	t_MaterialMetallic,
	t_MaterialRoughness,
	t_MaterialOcclusion,

	t_EnvironmentMap = 20,
	t_IrradianceMap,
	t_PrefilteredMap,

	t_GaussianMap = 30,
	t_IntegrationMap,

	// "Virtual" textures that don't exist in memory but can be computed from other textures
	t_Normal = D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT,
	t_Depth,
	t_Position
};

#define GBUFFER_BEGIN t_GBufferNormalDepth
#define GBUFFER_END (t_GBufferEntity + 1)
#define GBUFFER_COUNT (GBUFFER_END - GBUFFER_BEGIN)

enum SamplerSlot : unsigned
{
	s_PointSampler,
	s_TrilinearSampler,
	s_NormalDepthSampler,
	s_GaussianSampler,
	SamplerCount
};

std::array<D3D11_SAMPLER_DESC, SamplerCount> GetSamplerDescs();

// Precomputed maps

ShaderResourcePtr CreateGaussianMap();
ShaderResourcePtr CreateIntegrationMap(); // BRDF LUT for use in specular IBL

