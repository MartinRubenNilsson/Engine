#pragma once

class PhysX : public Singleton<PhysX>
{
public:
	PhysX();

	bool ConnectPvd();
	void DisconnectPvd();

	PxScene* GetScene();

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
	bool mySucceeded{ false };
};
