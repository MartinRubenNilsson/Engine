#pragma once

#define FULLSCREEN_VERTEX_COUNT 3
#define CUBEMAP_VERTEX_COUNT 14

struct alignas(16) CameraBuffer
{
	Matrix viewProj;
	Matrix invViewProj;
	Matrix invTransView;
	Vector4 position;
};

struct alignas(16) MeshBuffer
{
	Matrix matrix;
	Matrix matrixInvTrans;
	uint32_t entity[4];
};

struct alignas(16) LightBuffer
{
	Vector4 color{};	  // (r, g, b, [unused])
	Vector4 position{};   // (x, y, z, 1)
	Vector4 direction{};  // (x, y, z, 0)
	Vector4 parameters{}; // (range, constant attn, linear attn, quadratic attn)
	Vector4 coneAngles{}; // (inner, outer, [unused], [unused])
};

struct alignas(16) CubemapBuffer
{
	Matrix cubeFaceViewProjs[6];
};

enum CBufferSlots : unsigned
{
	b_Camera = 0,
	b_Mesh = 1,
	b_Light = 2,
	b_Cubemap = 3,
	CBufferCount,
};

enum TextureSlot : unsigned
{
	t_GBufferNormalDepth = 0,
	t_GBufferAlbedo,
	t_GBufferMetalRoughAo,
	t_GBufferEntity,

	t_SSAOTexture,
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

	// "Virtual" textures that don't exist in memory but can be computed from other textures
	t_Normal = D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT,
	t_Depth,
	t_Position
};

#define GBUFFER_BEGIN t_GBufferNormalDepth
#define GBUFFER_END (t_GBufferEntity + 1)
#define GBUFFER_COUNT (GBUFFER_END - GBUFFER_BEGIN)

#define MATERIAL_BEGIN t_MaterialAlbedo
#define MATERIAL_END (t_MaterialOcclusion + 1)
#define MATERIAL_COUNT (MATERIAL_END - MATERIAL_BEGIN)

enum SamplerSlot : unsigned
{
	s_PointSampler,
	s_TrilinearSampler,
	s_GaussianSampler,
	SamplerCount
};

std::span<const D3D11_SAMPLER_DESC> GetSamplerDescs();

