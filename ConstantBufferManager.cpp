#include "pch.h"
#include "ConstantBufferManager.h"

ConstantBufferManager::ConstantBufferManager()
	: Singleton()
	, myConstantBuffers
	{
		sizeof(CameraBuffer),
	}
{
	for (unsigned slot = 0; slot < Count; ++slot)
	{
		myConstantBuffers[slot].VSSetBuffer(slot);
		myConstantBuffers[slot].PSSetBuffer(slot);
	}
}


void ConstantBufferManager::WriteToBuffer(Slot aSlot, const void* someData)
{
	myConstantBuffers[aSlot].WriteToBuffer(someData);
}

ConstantBufferManager::operator bool() const
{
	return std::ranges::all_of(myConstantBuffers, &ConstantBuffer::operator bool);
}
