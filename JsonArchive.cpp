#include "pch.h"
#include "JsonArchive.h"
#include "Camera.h"

void JsonArchive::Save(const entt::registry& aRegistry)
{
	myJson.clear();

	entt::snapshot snapshot{ aRegistry };

	myKey = "entity";
	snapshot.entities(*this);

	myKey = "camera";
	snapshot.component<Camera>(*this);
}

void JsonArchive::operator()(std::underlying_type_t<entt::entity> aSize)
{
	myJson[myKey + "Count"] = aSize;
}

void JsonArchive::operator()(entt::entity anEntity)
{
	myJson[myKey].push_back(anEntity);
}
