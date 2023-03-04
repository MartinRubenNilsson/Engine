#pragma once

enum CBufferSlot : unsigned
{
	b_Camera = 0,
	b_Mesh = 1,
	b_Light = 2,
	b_Cubemap = 3,
};

enum TextureSlot : unsigned
{
	t_GBufferDepth = 0,
	t_GBufferVertexNormal = 1,
	t_GBufferPixelNormal = 2,
	t_GBufferAlbedo = 3,
	t_GBufferMetalRoughAo = 4,
	t_GBufferEntity = 5,

	t_MaterialAlbedo = 10,
	t_MaterialNormal = 11,
	t_MaterialMetallic = 12,
	t_MaterialRoughness = 13,
	t_MaterialOcclusion = 14,

	t_EnvironmentMap = 20,
	t_IrradianceMap = 21,
};

enum SamplerSlot : unsigned
{
	s_PointSampler = 0,
	s_TrilinearSampler = 1,
};

struct alignas(16) CameraBuffer
{
	Matrix viewProj;
	Matrix invViewProj;
	Vector4 position;
	Vector4 clipPlanes; // (near, far, [unused], [unused])
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

