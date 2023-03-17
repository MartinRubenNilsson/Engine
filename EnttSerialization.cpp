#include "pch.h"
#include "EnttSerialization.h"
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"

namespace
{
	struct OutputArchive
	{
		json& j;

		void operator()(entt::id_type) {}

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

		void operator()(entt::id_type& size)
		{
			size = static_cast<entt::id_type>(j.size());
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

void entt::to_json(json& j, const registry& aRegistry)
{
	j.clear();

	OutputArchive transforms{ j["transforms"] };
	OutputArchive cameras{ j["cameras"] };
	OutputArchive meshes{ j["meshes"] };
	OutputArchive materials{ j["materials"] };

	entt::snapshot{ aRegistry }
		.component<Transform>(transforms)
		.component<Camera>(cameras)
		.component<Mesh>(meshes)
		.component<Material>(materials);
}

void entt::from_json(const json& j, registry& aRegistry)
{
	aRegistry.clear();

	InputArchive transforms{ j.at("transforms") };
	InputArchive cameras{ j.at("cameras") };
	InputArchive meshes{ j.at("meshes") };
	InputArchive materials{ j.at("materials") };

	entt::snapshot_loader{ aRegistry }
		.component<Transform>(transforms)
		.component<Camera>(cameras)
		.component<Mesh>(meshes)
		.component<Material>(materials);
}
