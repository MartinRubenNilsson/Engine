#pragma once

class JsonArchive
{
public:
	JsonArchive() = default;
	JsonArchive(const fs::path&);

	void Serialize(const entt::registry&);
	void Deserialize(entt::registry&) const;

	const json& GetJson() const { return myJson; }

	explicit operator bool() const;

private:
	json myJson{};
};

