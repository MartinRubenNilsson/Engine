#pragma once

enum class CombineMode
{
	Average,
	Min,
	Multiply,
	Max,
	Count,
};

const char* ToString(CombineMode);

class PhysicMaterial
{
public:
	PhysicMaterial();
	PhysicMaterial(const PhysicMaterial&);
	PhysicMaterial& operator=(const PhysicMaterial&);

	void SetDynamicFriction(float);
	float GetDynamicFriction() const;
	void SetStaticFriction(float);
	float GetStaticFriction() const;
	void SetRestitution(float);
	float GetRestitution() const;
	void SetFrictionCombineMode(CombineMode);
	CombineMode GetFrictionCombineMode() const;
	void SetRestitutionCombineMode(CombineMode);
	CombineMode GetRestitutionCombineMode() const;

	PxMaterial* GetImpl();

	explicit operator bool() const;

private:
	PxPtr<PxMaterial> myImpl{};
};

void to_json(json&, const PhysicMaterial&);
void from_json(const json&, PhysicMaterial&);

namespace ImGui
{
	void Inspect(PhysicMaterial&);
}
