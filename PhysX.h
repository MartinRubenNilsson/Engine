#pragma once

class PhysX : public Singleton<PhysX>
{
public:
	PhysX();

	bool ConnectPvd();
	void DisconnectPvd();

	PxPhysics* GetPhysics();
	PxCpuDispatcher* GetCpuDispatcher();

	explicit operator bool() const;

private:
	PxDefaultAllocator myAllocator{};
	PxDefaultErrorCallback myErrorCallback{};
	PxPtr<PxFoundation> myFoundation{};
	PxPtr<PxPvdTransport> myPvdTransport{};
	PxPtr<PxPvd> myPvd{};
	PxPtr<PxPhysics> myPhysics{};
	PxPtr<PxDefaultCpuDispatcher> myCpuDispatcher{};
	bool mySucceeded{ false };
};

