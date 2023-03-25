#pragma once

class PhysX
{
public:
	PhysX();

	bool ConnectPvd();
	void DisconnectPvd();

	explicit operator bool() const;

private:
	PxDefaultAllocator myAllocator{};
	PxDefaultErrorCallback myErrorCallback{};
	PxPtr<PxFoundation> myFoundation{};
	PxPtr<PxPvdTransport> myPvdTransport{};
	PxPtr<PxPvd> myPvd{};
	bool mySucceeded{ false };
};

