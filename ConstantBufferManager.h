#pragma once
#include "ConstantBuffer.h"

struct CameraBuffer
{
	Matrix cameraMatrix;
	Matrix worldToClipMatrix;
};

struct MeshBuffer
{
	Matrix meshMatrix;
	Matrix meshMatrixInverseTranspose;
};

class ConstantBufferManager
{
public:
	enum Slot : unsigned
	{
		Camera,
		Mesh,
		Count
	};

	ConstantBufferManager();
	~ConstantBufferManager();

	static ConstantBufferManager& Get() { return *ourInstance; }

	void WriteConstantBuffer(Slot aSlot, const void* someData);

	operator bool() const;

private:
	ConstantBufferManager(const ConstantBufferManager&) = delete;
	ConstantBufferManager& operator=(const ConstantBufferManager&) = delete;
	ConstantBufferManager(ConstantBufferManager&&) = delete;
	ConstantBufferManager& operator=(ConstantBufferManager&&) = delete;

	static ConstantBufferManager* ourInstance;

	ConstantBuffer myConstantBuffers[Count];
};

#define DX11_WRITE_CBUFFER(slot, data) ConstantBufferManager::Get().WriteConstantBuffer(ConstantBufferManager::##slot, data)
