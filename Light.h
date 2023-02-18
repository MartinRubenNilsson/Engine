#pragma once

enum class LightType
{
	Directional,
	Point,
	Spot,
};

struct DirectionalLight
{
	Color color{}; // (r, g, b, intensity)
	Vector3 direction{ 0.f, 0.f, 1.f };
};

struct PointLight
{
	Color color{}; // (r, g, b, intensity)
	Vector3 position{};
	Vector4 parameters{ 5.0, 0.f, 1.f, 0.f }; // (range, constant attn, linear attn, quadratic attn)
};

struct SpotLight
{
	Color color{}; // (r, g, b, intensity)
	Vector3 position{};
	Vector3 direction{ 0.f, 0.f, 1.f };
	Vector4 parameters{ 5.0, 0.f, 1.f, 0.f }; // (range, constant attn, linear attn, quadratic attn)
	float innerAngle{ XM_PI / 8.f }; // half-angle in radians
	float outerAngle{ XM_PI / 4.f }; // half-angle in radians
};

class Light
{
public:
	Light() = default;
	Light(const aiLight&);

	LightType GetType() const;

	void SetLight(DirectionalLight);
	void SetLight(PointLight);
	void SetLight(SpotLight);

	template <class T>       T& GetLight()		 { return std::get<T>(myLight); }
	template <class T> const T& GetLight() const { return std::get<T>(myLight); }

private:
	std::variant<DirectionalLight, PointLight, SpotLight> myLight{};
};

namespace ImGui
{
	void InspectDirectionalLight(DirectionalLight&);
	void InspectPointLight(PointLight&);
	void InspectSpotLight(SpotLight&);
	void InspectLight(Light&);
}

