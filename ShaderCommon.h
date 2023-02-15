#pragma once

#define CBUFFER_SLOT_MESH 1

struct alignas(16) MeshBuffer
{
	Matrix meshMatrix;
	Matrix meshMatrixInverseTranspose;
	uint32_t meshEntity[4];
};