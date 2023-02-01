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
	ConstantBufferManager();
	~ConstantBufferManager();

	static ConstantBufferManager& Get() { return *ourInstance; }

	void WriteConstantBuffer(const CameraBuffer& aBuffer) { WriteConstantBuffer(Camera, &aBuffer); }
	void WriteConstantBuffer(const MeshBuffer& aBuffer) { WriteConstantBuffer(Mesh, &aBuffer); }

	operator bool() const;

private:
	enum Slot : unsigned
	{
		Camera,
		Mesh,
		Count
	};

	static ConstantBufferManager* ourInstance;

	ConstantBufferManager(const ConstantBufferManager&) = delete;
	ConstantBufferManager& operator=(const ConstantBufferManager&) = delete;
	ConstantBufferManager(ConstantBufferManager&&) = delete;
	ConstantBufferManager& operator=(ConstantBufferManager&&) = delete;

	void WriteConstantBuffer(Slot aSlot, const void* someData);

	ConstantBuffer myConstantBuffers[Count];
};

#define DX11_WRITE_CBUFFER(buffer) ConstantBufferManager::Get().WriteConstantBuffer(buffer)
