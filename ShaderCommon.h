#pragma once

#define CBUFFER_SLOT_CAMERA 0
#define CBUFFER_SLOT_MESH 1
#define CBUFFER_SLOT_LIGHT 2

#define TEXTURE_SLOT_GBUFFER 0
#define TEXTURE_SLOT_GBUFFER_ENTITY 5

struct alignas(16) MeshBuffer
{
	Matrix meshMatrix;
	Matrix meshMatrixInverseTranspose;
	uint32_t meshEntity[4];
};

struct alignas(16) LightBuffer
{
	Vector4 lightColor{};	   // (r, g, b, [unused])
	Vector4 lightPosition{};   // (x, y, z, 1)
	Vector4 lightDirection{};  // (x, y, z, 0)
	Vector4 lightParameters{}; // (range, constant attn, linear attn, quadratic attn)
	Vector4 lightConeAngles{}; // (inner, outer, [unused], [unused])
};

