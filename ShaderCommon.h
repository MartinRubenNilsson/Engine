#pragma once

#define CBUFFER_SLOT_MESH 1

#define TEXTURE_SLOT_GBUFFER_ENTITY 5

struct alignas(16) MeshBuffer
{
	Matrix meshMatrix;
	Matrix meshMatrixInverseTranspose;
	uint32_t meshEntity[4];
};