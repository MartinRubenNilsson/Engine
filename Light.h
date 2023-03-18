#pragma once

enum class LightType
{
	Directional,
	Point,
	Spot,
};

struct DirectionalLight {};

struct PointLight
{
	Vector4 parameters{ 5.0, 0.f, 1.f, 0.f }; // (range, constant attn, linear attn, quadratic attn)
};

struct SpotLight
{
	Vector4 parameters{ 5.0, 0.f, 1.f, 0.f }; // (range, constant attn, linear attn, quadratic attn)
	float innerAngle{ XM_PI / 8.f }; // half-angle in radians
	float outerAngle{ XM_PI / 4.f }; // half-angle in radians
};

using LightVariant = std::variant<DirectionalLight, PointLight, SpotLight>;

struct LightBuffer;

class Light
{
public:
	bool enabled{ true };

	Light() = default;
	Light(const aiLight&);

	LightType GetType() const;
	LightBuffer GetBuffer() const;

	void SetColor(const Color&);
	Color GetColor() const;
	void SetVariant(const LightVariant&);
	const LightVariant& GetVariant() const;

private:
	Color myColor{}; // (r, g, b, intensity)
	LightVariant myVariant{};
};

namespace ImGui
{
	void Inspect(Light&);
}
