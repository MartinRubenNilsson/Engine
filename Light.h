#pragma once

struct DirectionalLight
{
	Vector3 direction{};
	Vector3 up{};
};

struct PointLight
{
	Vector3 position{};
};

struct SpotLight
{
	Vector3 position{};
	Vector3 direction{};
	Vector3 up{};
	float innerAngle{};
	float outerAngle{};
};

class Light
{
public:
	Light() = default;
	Light(const aiLight&);

private:
	std::variant<DirectionalLight, PointLight, SpotLight> myLight{};
	Color myColor{};
};

