#include "pch.h"
#include "ConstantBufferManager.h"

ConstantBufferManager::ConstantBufferManager()
	: Singleton()
	, myConstantBuffers
	{
		sizeof(CameraBuffer),
		sizeof(MeshBuffer)
	}
{
	for (unsigned slot = 0; slot < Count; ++slot)
	{
		myConstantBuffers[slot].VSSetConstantBuffer(slot);
		myConstantBuffers[slot].PSSetConstantBuffer(slot);
	}
}


void ConstantBufferManager::WriteConstantBuffer(Slot aSlot, const void* someData)
{
	myConstantBuffers[aSlot].WriteConstantBuffer(someData);
}

ConstantBufferManager::operator bool() const
{
	return std::ranges::all_of(myConstantBuffers, &ConstantBuffer::operator bool);
}
