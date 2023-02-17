#pragma once

enum class LightType
{
	Directional,
	Point,
	Spot,
};

struct DirectionalLight
{
	Vector3 direction{ 0.f, 0.f, 1.f };
};

struct PointLight
{
	Vector3 position{};
	Vector4 parameters{ 5.0, 0.f, 1.f, 0.f }; // (range, constant attn, linear attn, quadratic attn)
};

struct SpotLight
{
	Vector3 position{};
	Vector3 direction{ 0.f, 0.f, 1.f };
	Vector4 parameters{ 5.0, 0.f, 1.f, 0.f }; // (range, constant attn, linear attn, quadratic attn)
	float outerAngle{ XM_PI / 4.f }; // half-angle in radians
	float innerAngle{ XM_PI / 8.f }; // half-angle in radians
};

class Light
{
public:
	Light() = default;
	Light(const aiLight&);

	LightType GetType() const;

	void SetColor(const Color&);
	Color GetColor() const { return myColor; }

	void SetLight(DirectionalLight);
	void SetLight(PointLight);
	void SetLight(SpotLight);

	template <class T>
	T GetLight() const { return std::get<T>(myLight); }

private:
	Color myColor{};
	std::variant<DirectionalLight, PointLight, SpotLight> myLight{};
};

namespace ImGui
{
	void InspectLight(Light& aLight);
}
