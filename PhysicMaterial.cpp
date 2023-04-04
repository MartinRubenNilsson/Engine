#include "pch.h"
#include "PhysicMaterial.h"
#include "PhysX.h"
#include "ImGuiCommon.h"

const char* ToString(CombineMode aMode)
{
	static constexpr std::array strings
	{
		"Average",
		"Min",
		"Multiply",
		"Max",
	};

	return strings.at(std::to_underlying(aMode));
}

namespace
{
	void Copy(PxMaterial* dst, const PxMaterial* src)
	{
		if (!dst || !src)
			return;

		dst->setDynamicFriction(src->getDynamicFriction());
		dst->setStaticFriction(src->getStaticFriction());
		dst->setRestitution(src->getRestitution());
		dst->setFrictionCombineMode(src->getFrictionCombineMode());
		dst->setRestitutionCombineMode(src->getRestitutionCombineMode());
	}
}

/*
* class PhysicMaterial
*/

PhysicMaterial::PhysicMaterial()
{
	myImpl.reset(PhysX::GetPhysics()->createMaterial(0.6f, 0.6f, 0.f));
}

PhysicMaterial::PhysicMaterial(const PhysicMaterial& other)
	: PhysicMaterial()
{
	Copy(myImpl.get(), other.myImpl.get());
}

PhysicMaterial& PhysicMaterial::operator=(const PhysicMaterial& other)
{
	Copy(myImpl.get(), other.myImpl.get());
	return *this;
}

void PhysicMaterial::SetDynamicFriction(float coef)
{
	if (myImpl)
		myImpl->setDynamicFriction(coef);
}

float PhysicMaterial::GetDynamicFriction() const
{
	return myImpl ? myImpl->getDynamicFriction() : 0.f;
}

void PhysicMaterial::SetStaticFriction(float coef)
{
	if (myImpl)
		myImpl->setStaticFriction(coef);
}

float PhysicMaterial::GetStaticFriction() const
{
	return myImpl ? myImpl->getStaticFriction() : 0.f;
}

void PhysicMaterial::SetRestitution(float coef)
{
	if (myImpl)
		myImpl->setRestitution(coef);
}

float PhysicMaterial::GetRestitution() const
{
	return myImpl ? myImpl->getRestitution() : 0.f;
}

void PhysicMaterial::SetFrictionCombineMode(CombineMode mode)
{
	if (myImpl)
		myImpl->setFrictionCombineMode(static_cast<PxCombineMode::Enum>(mode));
}

CombineMode PhysicMaterial::GetFrictionCombineMode() const
{
	return myImpl ? static_cast<CombineMode>(myImpl->getFrictionCombineMode()) : CombineMode{};
}

void PhysicMaterial::SetRestitutionCombineMode(CombineMode mode)
{
	if (myImpl)
		myImpl->setRestitutionCombineMode(static_cast<PxCombineMode::Enum>(mode));
}

CombineMode PhysicMaterial::GetRestitutionCombineMode() const
{
	return myImpl ? static_cast<CombineMode>(myImpl->getRestitutionCombineMode()) : CombineMode{};
}

PxMaterial* PhysicMaterial::GetImpl()
{
	return myImpl.get();
}

PhysicMaterial::operator bool() const
{
	return myImpl.operator bool();
}

void to_json(json& j, const PhysicMaterial& m)
{
	j["dynamicFriction"] = m.GetDynamicFriction();
	j["staticFriction"] = m.GetStaticFriction();
	j["restitution"] = m.GetRestitution();
	j["frictionCombine"] = m.GetFrictionCombineMode();
	j["restitutionCombine"] = m.GetRestitutionCombineMode();
}

void from_json(const json& j, PhysicMaterial& m)
{
	m.SetDynamicFriction(j.at("dynamicFriction"));
	m.SetStaticFriction(j.at("staticFriction"));
	m.SetRestitution(j.at("restitution"));
	m.SetFrictionCombineMode(j.at("frictionCombine"));
	m.SetRestitutionCombineMode(j.at("restitutionCombine"));
}

/*
* namespace ImGui
*/

void ImGui::Inspect(PhysicMaterial& m)
{
	float dynamicFriction = m.GetDynamicFriction();
	float staticFriction = m.GetStaticFriction();
	float restitution = m.GetRestitution();
	CombineMode frictionCombine = m.GetFrictionCombineMode();
	CombineMode restitutionCombine = m.GetRestitutionCombineMode();

	if (DragFloat("Dynamic Friction", &dynamicFriction, 0.01f, 0.f, FLT_MAX))
		m.SetDynamicFriction(dynamicFriction);
	if (DragFloat("Static Friction", &staticFriction, 0.01f, 0.f, FLT_MAX))
		m.SetStaticFriction(staticFriction);
	if (DragFloat("Restitution", &restitution, 0.01f, 0.f, 1.f))
		m.SetRestitution(restitution);
	if (Combo("Friction Combine", frictionCombine))
		m.SetFrictionCombineMode(frictionCombine);
	if (Combo("Restitution Combine", restitutionCombine))
		m.SetRestitutionCombineMode(restitutionCombine);
}
