#pragma once

PxVec3 ToPx(const Vector3&);

class PhysX : public Singleton<PhysX>
{
public:
	PhysX();

	bool ConnectPvd();
	void DisconnectPvd();

	PxPhysics* GetPhysics();
	PxScene* GetScene();
	PxControllerManager* GetControllerMgr();
	PxMaterial* GetDefaultMaterial();

	explicit operator bool() const;

private:
	PxDefaultAllocator myAllocator{};
	PxDefaultErrorCallback myErrorCallback{};
	PxPtr<PxFoundation> myFoundation{};
	PxPtr<PxPvdTransport> myPvdTransport{};
	PxPtr<PxPvd> myPvd{};
	PxPtr<PxPhysics> myPhysics{};
	PxPtr<PxDefaultCpuDispatcher> myCpuDispatcher{};
	PxPtr<PxScene> myScene{};
	PxPtr<PxControllerManager> myControllerMgr{};
	PxPtr<PxMaterial> myDefaultMaterial{};
	bool mySucceeded{ false };
};

#define PX_PHYSICS PhysX::Get().GetPhysics()
#define PX_SCENE PhysX::Get().GetScene()
