#pragma once

class PhysX
{
public:
	PhysX();

	explicit operator bool() const;

private:
	PxDefaultAllocator myAllocator{};
	PxDefaultErrorCallback myErrorCallback{};
	PxPtr<PxFoundation> myFoundation{};
	bool mySucceeded{ false };
};

