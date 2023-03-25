#pragma once

class PhysicsScene
{
public:
	PhysicsScene();

private:
	PxPtr<PxScene> myScene{};
};

