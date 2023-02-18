#include "pch.h"
#include "Light.h"

/*
* class Light
*/

Light::Light(const aiLight& aLight)
{
	switch (aLight.mType)
	{
	case aiLightSource_DIRECTIONAL:
	{
		DirectionalLight light{};
		std::memcpy(&light.color, &aLight.mColorDiffuse, sizeof(aiColor3D));
		std::memcpy(&light.direction, &aLight.mDirection, sizeof(aiVector3D));
		SetLight(light);
		break;
	}
	case aiLightSource_POINT:
	{
		PointLight light{};
		std::memcpy(&light.color, &aLight.mColorDiffuse, sizeof(aiColor3D));
		std::memcpy(&light.position, &aLight.mPosition, sizeof(aiVector3D));
		light.parameters = { 5.0f, aLight.mAttenuationConstant, aLight.mAttenuationLinear, aLight.mAttenuationQuadratic };
		SetLight(light);
		break;
	}
	case aiLightSource_SPOT:
	{
		SpotLight light{};
		std::memcpy(&light.color, &aLight.mColorDiffuse, sizeof(aiColor3D));
		std::memcpy(&light.position, &aLight.mPosition, sizeof(aiVector3D));
		std::memcpy(&light.direction, &aLight.mDirection, sizeof(aiVector3D));
		light.parameters = { 5.0f, aLight.mAttenuationConstant, aLight.mAttenuationLinear, aLight.mAttenuationQuadratic };
		light.outerAngle = aLight.mAngleOuterCone;
		light.innerAngle = aLight.mAngleInnerCone;
		SetLight(light);
		break;
	}
	default:
		assert(false);
	}
}

void Light::SetLight(DirectionalLight aLight)
{
	aLight.color = Vector4::Max(aLight.color, Vector4::Zero);
	aLight.direction.Normalize();
	myLight = aLight;
}

void Light::SetLight(PointLight aLight)
{
	aLight.color = Vector4::Max(aLight.color, Vector4::Zero);
	aLight.parameters = Vector4::Max(aLight.parameters, Vector4::Zero);
	myLight = aLight;
}

void Light::SetLight(SpotLight aLight)
{
	aLight.color = Vector4::Max(aLight.color, Vector4::Zero);
	aLight.direction.Normalize();
	aLight.parameters = Vector4::Max(aLight.parameters, Vector4::Zero);
	aLight.outerAngle = std::clamp(aLight.outerAngle, 0.f, XM_PIDIV2);
	aLight.innerAngle = std::clamp(aLight.innerAngle, 0.f, aLight.outerAngle);
	myLight = aLight;
}

LightType Light::GetType() const
{
	return static_cast<LightType>(myLight.index());
}

/*
* namespace ImGui
*/

void ImGui::InspectLight(Light& aLight)
{
	auto type{ static_cast<int>(aLight.GetType()) };
	Combo("Type", &type, "Directional\0Point\0Spot\0\0");

	switch (aLight.GetType())
	{
	case LightType::Directional:
	{
		auto light{ aLight.GetLight<DirectionalLight>() };
		ColorEdit3("Color", &light.color.x);
		aLight.SetLight(light);
		break;
	}
	case LightType::Point:
	{
		auto light{ aLight.GetLight<PointLight>() };
		ColorEdit3("Color", &light.color.x);
		DragFloat("Range", &light.parameters.x, 0.1f, 0.f, FLT_MAX);
		DragFloat3("Attenuation", &light.parameters.y, 0.1f, 0.f, FLT_MAX);
		aLight.SetLight(light);
		break;
	}
	case LightType::Spot:
	{
		auto light{ aLight.GetLight<SpotLight>() };
		ColorEdit3("Color", &light.color.x);
		DragFloat("Range", &light.parameters.x, 0.1f, 0.f, FLT_MAX);
		DragFloat3("Attenuation", &light.parameters.y, 0.1f, 0.f, FLT_MAX);
		DragFloat("Outer Angle", &light.outerAngle, 0.01f, 0.f, XM_PIDIV2);
		DragFloat("Inner Angle", &light.innerAngle, 0.01f, 0.f, light.outerAngle);
		aLight.SetLight(light);
		break;
	}
	}
}
