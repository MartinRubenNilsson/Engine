#include "pch.h"
#include "EnttSerialization.h"
#include "Tags.h"
#include "Transform.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"

// Physics
#include "CharacterController.h"

// #include "Light.h" // todo: lights

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

		template <>
		void operator()(entt::entity anEntity, const std::string& aName)
		{
			json& component = pool.emplace_back();
			component["entity"] = anEntity;
			component["name"] = aName;
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

		template <>
		void operator()(entt::entity& anEntity, std::string& aName)
		{
			const json& component = pool.at(index++);
			component.at("entity").get_to(anEntity);
			component.at("name").get_to(aName);
		}
	};

	template<class Component>
	void ToJson(std::string_view name, json& someJson, const entt::snapshot& aSnapshot)
	{
		OutputArchive archive{ someJson[name] };
		aSnapshot.component<Component>(archive);
	}

	template <class Component>
	void FromJson(std::string_view name, const json& someJson, entt::snapshot_loader& aSnapshot)
	{
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

	ToJson<std::string>("name", someJson, snapshot);
	ToJson<Tag::Selected>("selected", someJson, snapshot);
	ToJson<Transform>("transform", someJson, snapshot);
	ToJson<Material>("material", someJson, snapshot);
	ToJson<Mesh>("mesh", someJson, snapshot);
	ToJson<Camera>("camera", someJson, snapshot);
	ToJson<CharacterController>("characterController", someJson, snapshot);
}

void entt::from_json(const json& someJson, registry& aRegistry)
{
	aRegistry.clear();

	entt::snapshot_loader snapshot{ aRegistry };

	FromJson<std::string>("name", someJson, snapshot);
	FromJson<Tag::Selected>("selected", someJson, snapshot);
	FromJson<Transform>("transform", someJson, snapshot);
	FromJson<Material>("material", someJson, snapshot);
	FromJson<Mesh>("mesh", someJson, snapshot);
	FromJson<Camera>("camera", someJson, snapshot);
	FromJson<CharacterController>("characterController", someJson, snapshot);
}
