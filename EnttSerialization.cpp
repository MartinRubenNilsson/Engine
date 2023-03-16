#include "pch.h"
#include "EnttSerialization.h"
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

void entt::to_json(json& j, const registry& reg)
{
	j.clear();

	OutputArchive transforms{ j["transforms"] };
	OutputArchive cameras{ j["cameras"] };

	entt::snapshot{ reg }
		.component<Transform>(transforms)
		.component<Camera>(cameras);
}

void entt::from_json(const json& j, registry& reg)
{
	reg.clear();

	InputArchive transforms{ j.at("transforms") };
	InputArchive cameras{ j.at("cameras") };

	entt::snapshot_loader{ reg }
		.component<Transform>(transforms)
		.component<Camera>(cameras);
}
