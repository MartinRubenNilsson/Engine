#pragma once

class JsonArchive
{
public:
	void Save(const entt::registry&);

	const json& GetJson() const { return myJson; }

	void operator()(std::underlying_type_t<entt::entity>);

	template <class Component>
	void operator()(entt::entity e, const Component& c)
	{
		myJson[myComponentName].emplace_back(c)["entity"] = e;
	}

private:
	json myJson{};
	std::string myComponentName{};
};

