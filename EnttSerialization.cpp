#include "pch.h"
#include "EnttSerialization.h"
#include "Tags.h"
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
// #include "Light.h"

// todo: lights

namespace
{
	struct OutputArchive
	{
		json& pool;

		void operator()(entt::id_type) {}

		void operator()(entt::entity anEntity)
		{
			pool.emplace_back(anEntity);
		}

		template <class Component>
		void operator()(entt::entity anEntity, const Component& aComponent)
		{
			json& component = pool.emplace_back(aComponent);
			component["entity"] = anEntity;
		}
	};

	struct InputArchive
	{
		const json& pool;
		size_t index = 0;

		void operator()(entt::id_type& aSize)
		{
			aSize = static_cast<entt::id_type>(pool.size());
		}

		void operator()(entt::entity& anEntity)
		{
			pool.at(index++).get_to(anEntity);
		}

		template <class Component>
		void operator()(entt::entity& anEntity, Component& aComponent)
		{
			const json& component = pool.at(index++);
			component.at("entity").get_to(anEntity);
			component.get_to(aComponent);
		}
	};

	template<class Component>
	void ToJson(json& someJson, const entt::snapshot& aSnapshot)
	{
		static constexpr std::string_view name{ entt::type_name<Component>::value() };

		OutputArchive archive{ someJson[name] };
		aSnapshot.component<Component>(archive);
	}

	template <class Component>
	void FromJson(const json& someJson, entt::snapshot_loader& aSnapshot)
	{
		static constexpr std::string_view name{ entt::type_name<Component>::value() };

		if (someJson.contains(name))
		{
			InputArchive archive{ someJson.at(name) };
			aSnapshot.component<Component>(archive);
		}
	}
}

void entt::to_json(json& someJson, const registry& aRegistry)
{
	someJson.clear();

	entt::snapshot snapshot{ aRegistry };

	ToJson<Tag::Selected>(someJson, snapshot);
	ToJson<Transform>(someJson, snapshot);
	ToJson<Material>(someJson, snapshot);
	ToJson<Mesh>(someJson, snapshot);
	ToJson<Camera>(someJson, snapshot);
}

void entt::from_json(const json& someJson, registry& aRegistry)
{
	aRegistry.clear();

	entt::snapshot_loader snapshot{ aRegistry };

	FromJson<Tag::Selected>(someJson, snapshot);
	FromJson<Transform>(someJson, snapshot);
	FromJson<Material>(someJson, snapshot);
	FromJson<Mesh>(someJson, snapshot);
	FromJson<Camera>(someJson, snapshot);
}
