#include "pch.h"
#include "PhysicsScene.h"
#include "PhysX.h"

PhysicsScene::PhysicsScene()
{
	PxPhysics* physics = PhysX::Get().GetPhysics();

	PxSceneDesc desc{ physics->getTolerancesScale() };
	desc.gravity = { 0.0f, -9.81f, 0.0f };
	desc.cpuDispatcher = PhysX::Get().GetCpuDispatcher();
	desc.filterShader = PxDefaultSimulationFilterShader;

	// todo: create scene from physics

	/*gScene = gPhysics->createScene(sceneDesc);*/
}
