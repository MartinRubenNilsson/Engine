#pragma once

enum CBufferSlot : unsigned
{
	CBufferCamera,
	CBufferMesh,
	CBufferLight,
	CBufferCount,
};

enum SamplerSlot : unsigned
{
	PointSampler,
	TrilinearSampler,
	SamplerCount
};

#define TEXTURE_SLOT_GBUFFER 0
#define TEXTURE_SLOT_GBUFFER_DEPTH 0
#define TEXTURE_SLOT_GBUFFER_ENTITY 5

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

