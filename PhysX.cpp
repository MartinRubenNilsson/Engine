#include "pch.h"
#include "PhysX.h"

#define PVD_HOST "127.0.0.1"
#define NUM_THREADS 2

PxVec3 ToPx(const Vector3& v)
{
	return { v.x, v.y, v.z };
}

PxExtendedVec3 ToPxEx(const Vector3& v)
{
	return { static_cast<double>(v.x), static_cast<double>(v.y), static_cast<double>(v.z) };
}

Vector3 FromPx(const PxExtendedVec3& v)
{
	return { static_cast<float>(v.x), static_cast<float>(v.y), static_cast<float>(v.z) };
}

/*
* class PhysX
*/

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

	PxSceneDesc desc{ myPhysics->getTolerancesScale() };
	desc.gravity = { 0.0f, -9.81f, 0.0f };
	desc.cpuDispatcher = myCpuDispatcher.get();
	desc.filterShader = PxDefaultSimulationFilterShader;
	if (!desc.isValid())
		return;

	myScene.reset(myPhysics->createScene(desc));
	if (!myScene)
		return;

#ifdef _DEBUG
	if (auto client = myScene->getScenePvdClient())
	{
		client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	}
#endif

	myControllerMgr.reset(PxCreateControllerManager(*myScene));
	if (!myControllerMgr)
		return;

	myDefaultMaterial.reset(myPhysics->createMaterial(0.6f, 0.6f, 0.f));
	if (!myDefaultMaterial)
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

PxScene* PhysX::GetScene()
{
	return myScene.get();
}

PxControllerManager* PhysX::GetControllerMgr()
{
	return myControllerMgr.get();
}

PxMaterial* PhysX::GetDefaultMaterial()
{
	return myDefaultMaterial.get();
}

PhysX::operator bool() const
{
	return mySucceeded;
}
