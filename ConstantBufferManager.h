#pragma once
#include "ConstantBuffer.h"

struct alignas(16) CameraBuffer
{
	Matrix cameraViewProjMatrix;
	Vector4 cameraPosition;
};

class ConstantBufferManager : public Singleton<ConstantBufferManager>
{
public:
	ConstantBufferManager();

	void WriteToBuffer(const CameraBuffer& aBuffer) { WriteToBuffer(Camera, &aBuffer); }

	explicit operator bool() const;

private:
	enum Slot : unsigned
	{
		Camera,
		Count
	};

	void WriteToBuffer(Slot aSlot, const void* someData);

	ConstantBuffer myConstantBuffers[Count];
};

#define DX11_WRITE_CONSTANT_BUFFER(buffer) ConstantBufferManager::Get().WriteToBuffer(buffer)
