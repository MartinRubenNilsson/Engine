#include "pch.h"
#include "PhysX.h"

#define PVD_HOST "127.0.0.1"

PhysX::PhysX()
{
	myFoundation.reset(PxCreateFoundation(PX_PHYSICS_VERSION, myAllocator, myErrorCallback));
	if (!myFoundation)
		return;

#ifdef _DEBUG
	myPvdTransport.reset(PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10));
	if (!myPvdTransport)
		return;

	myPvd.reset(PxCreatePvd(*myFoundation));
	if (!myPvd)
		return;
#endif

	mySucceeded = true;
}

bool PhysX::ConnectPvd()
{
	if (myPvd && myPvdTransport)
		return myPvd->connect(*myPvdTransport, PxPvdInstrumentationFlag::eALL);
	return false;
}

void PhysX::DisconnectPvd()
{
	if (myPvd)
		myPvd->disconnect();
}

PhysX::operator bool() const
{
	return mySucceeded;
}
