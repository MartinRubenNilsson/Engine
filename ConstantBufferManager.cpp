#include "pch.h"
#include "ConstantBufferManager.h"

ConstantBufferManager* ConstantBufferManager::ourInstance = nullptr;

ConstantBufferManager::ConstantBufferManager()
	: myConstantBuffers
	{
		sizeof(CameraBuffer),
		sizeof(MeshBuffer)
	}
{
	assert(!ourInstance);
	ourInstance = this;

	for (unsigned slot = 0; slot < Count; ++slot)
	{
		myConstantBuffers[slot].VSSetConstantBuffer(slot);
		myConstantBuffers[slot].PSSetConstantBuffer(slot);
	}
}

ConstantBufferManager::~ConstantBufferManager()
{
	ourInstance = nullptr;
}

void ConstantBufferManager::WriteConstantBuffer(Slot aSlot, const void* someData)
{
	myConstantBuffers[aSlot].WriteConstantBuffer(someData);
}

ConstantBufferManager::operator bool() const
{
	return std::ranges::all_of(myConstantBuffers, &ConstantBuffer::operator bool);
}
