#include "pch.h"
#include "PhysX.h"

#define PVD_HOST "127.0.0.1"
#define NUM_THREADS 2

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

	bool trackAllocs = true;
#else
	bool trackAllocs = false;
#endif

	myPhysics.reset(PxCreateBasePhysics(PX_PHYSICS_VERSION, *myFoundation, PxTolerancesScale{}, trackAllocs, myPvd.get()));
	if (!myPhysics)
		return;

	myCpuDispatcher.reset(PxDefaultCpuDispatcherCreate(NUM_THREADS));
	if (!myCpuDispatcher)
		return;

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

PxPhysics* PhysX::GetPhysics()
{
	return myPhysics.get();
}

PxCpuDispatcher* PhysX::GetCpuDispatcher()
{
	return myCpuDispatcher.get();
}

PhysX::operator bool() const
{
	return mySucceeded;
}
