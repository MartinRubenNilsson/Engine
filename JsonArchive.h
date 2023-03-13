#pragma once

class JsonArchive
{
public:
	void Save(const entt::registry&);

	const json& GetJson() const { return myJson; }

	void operator()(std::underlying_type_t<entt::entity>);
	void operator()(entt::entity);

	template <class Component>
	void operator()(entt::entity, const Component& c)
	{
		myJson[myKey].emplace_back(c);
	}

private:
	json myJson{};
	std::string myKey{};
};

