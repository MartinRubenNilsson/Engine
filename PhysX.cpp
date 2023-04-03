#include "pch.h"
#include "PhysX.h"

#define PVD_HOST "127.0.0.1"
#define NUM_THREADS 2

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
	PxPvdTransport* thePvdTransport = nullptr;
	PxPvd* thePvd = nullptr;
	PxPhysics* thePhysics = nullptr;
	PxScene* theScene = nullptr;
	PxControllerManager* theControllerMgr = nullptr;
	PxMaterial* theDefaultMaterial = nullptr;
}

/*
* namespace PhysX
*/

bool PhysX::Create()
{
	static PxDefaultAllocator allocator{};
	static PxDefaultErrorCallback errorCallback{};

	static PxPtr<PxFoundation> foundation{ PxCreateFoundation(PX_PHYSICS_VERSION, allocator, errorCallback) };
	if (!foundation)
		return false;

#ifdef _DEBUG
	static PxPtr<PxPvdTransport> pvdTransport{ PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10) };
	if (!pvdTransport)
		return false;

	static PxPtr<PxPvd> pvd{ PxCreatePvd(*foundation) };
	if (!pvd)
		return false;

	bool trackAllocs = true;
#else
	bool trackAllocs = false;
#endif

	static PxPtr<PxPhysics> physics{ PxCreateBasePhysics(PX_PHYSICS_VERSION, *foundation, {}, trackAllocs, pvd.get()) };
	if (!physics)
		return false;

	static PxPtr<PxDefaultCpuDispatcher> cpuDispatcher{ PxDefaultCpuDispatcherCreate(NUM_THREADS) };
	if (!cpuDispatcher)
		return false;

	PxSceneDesc desc{ physics->getTolerancesScale() };
	desc.gravity = { 0.0f, -9.81f, 0.0f };
	desc.cpuDispatcher = cpuDispatcher.get();
	desc.filterShader = PxDefaultSimulationFilterShader;
	if (!desc.isValid())
		return false;

	static PxPtr<PxScene> scene{ physics->createScene(desc) };
	if (!scene)
		return false;

#ifdef _DEBUG
	if (PxPvdSceneClient* client = scene->getScenePvdClient())
	{
		client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		client->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	}
#endif

	static PxPtr<PxControllerManager> controllerMgr{ PxCreateControllerManager(*scene) };
	if (!controllerMgr)
		return false;

	static PxPtr<PxMaterial> defaultMaterial{ physics->createMaterial(0.6f, 0.6f, 0.f) };
	if (!defaultMaterial)
		return false;

	thePvdTransport = pvdTransport.get();
	thePvd = pvd.get();
	thePhysics = physics.get();
	theScene = scene.get();
	theControllerMgr = controllerMgr.get();
	theDefaultMaterial = defaultMaterial.get();

	return true;
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
}
