#include "pch.h"
#include "JsonArchive.h"
#include "Transform.h"
#include "Camera.h"

void JsonArchive::Save(const entt::registry& aRegistry)
{
	myJson.clear();

	entt::snapshot snapshot{ aRegistry };

	myComponentName = "transforms";
	snapshot.component<Transform>(*this);

	myComponentName = "cameras";
	snapshot.component<Camera>(*this);
}

void JsonArchive::operator()(std::underlying_type_t<entt::entity> aSize)
{
	myJson["num_" + myComponentName] = aSize;
}
