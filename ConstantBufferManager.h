#pragma once
#include "ConstantBuffer.h"

struct alignas(16) CameraBuffer
{
	Matrix cameraMatrix;
	Matrix worldToClipMatrix;
};

struct alignas(16) MeshBuffer
{
	Matrix meshMatrix;
	Matrix meshMatrixInverseTranspose;
};

class ConstantBufferManager : public Singleton<ConstantBufferManager>
{
public:
	ConstantBufferManager();

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

	void WriteConstantBuffer(Slot aSlot, const void* someData);

	ConstantBuffer myConstantBuffers[Count];
};

#define DX11_WRITE_CONSTANT_BUFFER(buffer) ConstantBufferManager::Get().WriteConstantBuffer(buffer)
