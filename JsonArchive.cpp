#include "pch.h"
#include "JsonArchive.h"
#include "Transform.h"
#include "Camera.h"

namespace
{
	struct OutputArchive
	{
		json& j;
		std::string type{}; // Type of component

		void operator()(std::underlying_type_t<entt::entity> aSize)
		{
			j["num_" + type] = aSize;
		}

		template <class T>
		void operator()(entt::entity anEntity, const T& aComponent)
		{
			json& component = j[type].emplace_back(aComponent);
			component["entity"] = anEntity;
		}
	};

	struct InputArchive
	{
		const json& j;
		std::string type{};  // Type of component
		size_t index{}; // Index of component

		void operator()(std::underlying_type_t<entt::entity>& aSize)
		{
			j.at("num_" + type).get_to(aSize);
		}


		template <class T>
		void operator()(entt::entity& anEntity, T& aComponent)
		{
			const json& component = j.at(type).at(index++);
			component.at("entity").get_to(anEntity);
			component.get_to(aComponent);
		}
	};
}

JsonArchive::JsonArchive(const fs::path& aPath)
{
	std::ifstream file{ aPath };
	if (!file)
		return;

	myJson = json::parse(file);
}

void JsonArchive::Serialize(const entt::registry& aRegistry)
{
	myJson.clear();

	OutputArchive archive{ myJson };

	entt::snapshot snapshot{ aRegistry };

	archive.type = "transforms";
	snapshot.component<Transform>(archive);

	archive.type = "cameras";
	snapshot.component<Camera>(archive);
}

void JsonArchive::Deserialize(entt::registry& aRegistry) const
{
	InputArchive archive{ myJson };

	entt::snapshot_loader snapshot{ aRegistry };

	archive.type = "transforms";
	archive.index = 0;
	snapshot.component<Transform>(archive);

	archive.type = "cameras";
	archive.index = 0;
	snapshot.component<Camera>(archive);
}

JsonArchive::operator bool() const
{
	return !myJson.empty();
}
