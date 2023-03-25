#include "pch.h"
#include "PhysX.h"

PhysX::PhysX()
{
	myFoundation.reset(PxCreateFoundation(
		PX_PHYSICS_VERSION,
		myAllocator,
		myErrorCallback
	));
	if (!myFoundation)
		return;

	mySucceeded = true;
}

PhysX::operator bool() const
{
	return mySucceeded;
}
