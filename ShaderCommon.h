#pragma once

#define FULLSCREEN_VERTEX_COUNT 3
#define CUBEMAP_VERTEX_COUNT 14

struct alignas(16) CameraBuffer
{
	Matrix viewProj;
	Matrix invViewProj;
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
	Vector4 color{};	   // (r, g, b, [unused])
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
	t_GBufferDepth = 0,
	t_GBufferNormal,
	t_GBufferAlbedo,
	t_GBufferMetalRoughAo,
	t_GBufferEntity,

	t_LightingBuffer,

	t_MaterialAlbedo = 10,
	t_MaterialNormal,
	t_MaterialMetallic,
	t_MaterialRoughness,
	t_MaterialOcclusion,

	t_EnvironmentMap = 20,
	t_IrradianceMap,
	t_PrefilteredMap,
};

#define GBUFFER_BEGIN t_GBufferDepth
#define GBUFFER_END t_GBufferEntity + 1

enum SamplerSlot : unsigned
{
	s_PointSampler = 0,
	s_TrilinearSampler = 1,
};

