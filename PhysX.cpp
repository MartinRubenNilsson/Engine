#include "pch.h"
#include "PhysX.h"

#define PVD_HOST "127.0.0.1"
#define NUM_THREADS 2

#define PX_RELEASE(x) if (x) { x->release(); x = nullptr; }
#define PX_DELETE(x) if (x) { delete x; x = nullptr; }

PxVec3 ToPx(const Vector3& v)
{
	return { v.x, v.y, v.z };
}

Vector3 FromPx(const PxVec3& v)
{
	return { v.x, v.y, v.z };
}

PxQuat ToPx(const Quaternion& q)
{
	return { q.x, q.y, q.z, q.w };
}

Quaternion FromPx(const PxQuat& q)
{
	return { q.x, q.y, q.z, q.w };
}

PxExtendedVec3 ToPxEx(const Vector3& v)
{
	return { static_cast<double>(v.x), static_cast<double>(v.y), static_cast<double>(v.z) };
}

Vector3 FromPxEx(const PxExtendedVec3& v)
{
	return { static_cast<float>(v.x), static_cast<float>(v.y), static_cast<float>(v.z) };
}

namespace
{
	PxDefaultAllocator* theAllocator = nullptr;
	PxDefaultErrorCallback* theErrorCallback = nullptr;

	PxFoundation* theFoundation = nullptr;
	PxPvdTransport* thePvdTransport = nullptr;
	PxPvd* thePvd = nullptr;
	PxPhysics* thePhysics = nullptr;
	PxScene* theScene = nullptr;
	PxDefaultCpuDispatcher* theCpuDispatcher = nullptr;
	PxControllerManager* theControllerMgr = nullptr;
	PxMaterial* theDefaultMaterial = nullptr;
}

/*
* namespace PhysX
*/

bool PhysX::Create()
{
	theAllocator = new PxDefaultAllocator{};
	theErrorCallback = new PxDefaultErrorCallback{};

	theFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *theAllocator, *theErrorCallback);
	if (!theFoundation)
		return false;

#ifdef _DEBUG
	thePvdTransport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	if (!thePvdTransport)
		return false;

	thePvd = PxCreatePvd(*theFoundation);
	if (!thePvd)
		return false;

	bool trackAllocs = true;
#else
	bool trackAllocs = false;
#endif

	thePhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *theFoundation, {}, trackAllocs, thePvd);
	if (!thePhysics)
		return false;

	theCpuDispatcher = PxDefaultCpuDispatcherCreate(NUM_THREADS);
	if (!theCpuDispatcher)
		return false;

	PxSceneDesc desc{ thePhysics->getTolerancesScale() };
	desc.gravity = { 0.0f, -9.81f, 0.0f };
	desc.cpuDispatcher = theCpuDispatcher;
	desc.filterShader = PxDefaultSimulationFilterShader;
	if (!desc.isValid())
		return false;

	theScene =  thePhysics->createScene(desc);
	if (!theScene)
		return false;

#ifdef _DEBUG
	if (PxPvdSceneClient* client = theScene->getScenePvdClient())
	{
		client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	}
#endif

	theControllerMgr = PxCreateControllerManager(*theScene);
	if (!theControllerMgr)
		return false;

	theDefaultMaterial = thePhysics->createMaterial(0.6f, 0.6f, 0.f);
	if (!theDefaultMaterial)
		return false;

	return true;
}

void PhysX::Destroy()
{
	PX_RELEASE(theDefaultMaterial)
	PX_RELEASE(theControllerMgr)
	PX_RELEASE(theCpuDispatcher)
	PX_RELEASE(theScene)
	PX_RELEASE(thePhysics)
	PX_RELEASE(thePvd)
	PX_RELEASE(thePvdTransport)
	PX_RELEASE(theFoundation)

	PX_DELETE(theErrorCallback)
	PX_DELETE(theAllocator)
}

PxPhysics* PhysX::GetPhysics()
{
	return thePhysics;
}

PxScene* PhysX::GetScene()
{
	return theScene;
}

PxControllerManager* PhysX::GetControllerMgr()
{
	return theControllerMgr;
}

PxMaterial* PhysX::GetDefaultMaterial()
{
	return theDefaultMaterial;
}

/*
* namespace ImGui
*/

void ImGui::PhysX()
{
	bool connected = thePvd->isConnected();
	if (Checkbox("Connect Visual Debugger", &connected))
	{
		if (connected)
			thePvd->connect(*thePvdTransport, PxPvdInstrumentationFlag::eALL);
		else
			thePvd->disconnect();
	}

	Value("Static Rigidbodies", theScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC));
	Value("Dynamic Rigidbodies", theScene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC));
	Value("Character Controllers", theControllerMgr->getNbControllers());
}
