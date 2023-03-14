#include "pch.h"
#include "JsonArchive.h"
#include "Camera.h"

void JsonArchive::Save(const entt::registry& aRegistry)
{
	myJson.clear();

	entt::snapshot snapshot{ aRegistry };

	myType = "entites";
	snapshot.entities(*this);

	myType = "cameras";
	snapshot.component<Camera>(*this);
}

void JsonArchive::operator()(std::underlying_type_t<entt::entity> aSize)
{
	myJson["num_" + myType] = aSize;
}

void JsonArchive::operator()(entt::entity anEntity)
{
	myJson[myType].push_back(anEntity);
}
