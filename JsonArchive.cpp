#include "pch.h"
#include "JsonArchive.h"
#include "Transform.h"
#include "Camera.h"

namespace
{
	struct OutputArchive
	{
		json& j;

		void operator()(uint32_t) {}

		template <class Component>
		void operator()(entt::entity e, const Component& c)
		{
			json& jc = j.emplace_back(c);
			jc["entity"] = e;
		}
	};

	struct InputArchive
	{
		const json& j;
		size_t i = 0;

		void operator()(uint32_t& size)
		{
			size = static_cast<uint32_t>(j.size());
		}

		template <class Component>
		void operator()(entt::entity& e, Component& c)
		{
			const json& ji = j.at(i++);
			ji.at("entity").get_to(e);
			ji.get_to(c);
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

	OutputArchive transforms{ myJson["transforms"] };
	OutputArchive cameras{ myJson["cameras"] };

	entt::snapshot{ aRegistry }
		.component<Transform>(transforms)
		.component<Camera>(cameras);
}

void JsonArchive::Deserialize(entt::registry& aRegistry) const
{
	InputArchive transforms{ myJson.at("transforms") };
	InputArchive cameras{ myJson.at("cameras") };

	entt::snapshot_loader{ aRegistry }
		.component<Transform>(transforms)
		.component<Camera>(cameras);
}

JsonArchive::operator bool() const
{
	return !myJson.empty();
}
