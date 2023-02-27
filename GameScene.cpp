#include "pch.h"
#include "GameScene.h"

GameScene::GameScene()
{
	myRegistry.on_construct<Transform::Ptr>().connect<&GameScene::OnConstructTransform>(this);
	myRegistry.on_destroy<Transform::Ptr>().connect<&GameScene::OnDestroyTransform>(this);
}

void GameScene::OnConstructTransform(entt::registry& aRegistry, entt::entity anEntity)
{
	myTransformToEntity.emplace(aRegistry.get<Transform::Ptr>(anEntity), anEntity);
}

void GameScene::OnDestroyTransform(entt::registry& aRegistry, entt::entity anEntity)
{
	auto transform{ aRegistry.get<Transform::Ptr>(anEntity) };
	auto children{ transform->GetChildren() };

	for (const auto& child : children)
	{
		aRegistry.destroy(myTransformToEntity.at(child));
		myTransformToEntity.erase(child);
	}

	transform->SetParent(nullptr, false);
}
