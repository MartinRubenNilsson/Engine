#include "pch.h"
#include "Light.h"

namespace
{
	void Unpremultiply(Color& aColor)
	{
		float rgbMax = std::max({ aColor.x, aColor.y, aColor.z });
		if (rgbMax <= 1.f)
			return;
		aColor.x /= rgbMax;
		aColor.y /= rgbMax;
		aColor.z /= rgbMax;
		aColor.w *= rgbMax;
	}
}

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
		light.innerAngle = aLight.mAngleInnerCone;
		light.outerAngle = aLight.mAngleOuterCone;
		SetLight(light);
		break;
	}
	default:
		Debug::Println("Warning: Unrecognized light type");
		break;
	}
}

LightType Light::GetType() const
{
	return static_cast<LightType>(myLight.index());
}

void Light::SetLight(DirectionalLight aLight)
{
	aLight.color = Vector4::Max(aLight.color, Vector4::Zero);
	Unpremultiply(aLight.color);
	aLight.direction.Normalize();
	myLight = aLight;
}

void Light::SetLight(PointLight aLight)
{
	aLight.color = Vector4::Max(aLight.color, Vector4::Zero);
	Unpremultiply(aLight.color);
	aLight.parameters = Vector4::Max(aLight.parameters, Vector4::Zero);
	myLight = aLight;
}

void Light::SetLight(SpotLight aLight)
{
	aLight.color = Vector4::Max(aLight.color, Vector4::Zero);
	Unpremultiply(aLight.color);
	aLight.direction.Normalize();
	aLight.parameters = Vector4::Max(aLight.parameters, Vector4::Zero);
	aLight.outerAngle = std::clamp(aLight.outerAngle, 0.f, XM_PIDIV2);
	aLight.innerAngle = std::clamp(aLight.innerAngle, 0.f, aLight.outerAngle);
	myLight = aLight;
}

/*
* namespace ImGui
*/

void ImGui::InspectDirectionalLight(DirectionalLight& aLight)
{
	ColorEdit3("Color", &aLight.color.x);
	DragFloat("Intensity", &aLight.color.w, 0.1f, 0.f, FLT_MAX);
}

void ImGui::InspectPointLight(PointLight& aLight)
{
	ColorEdit3("Color", &aLight.color.x);
	DragFloat("Intensity", &aLight.color.w, 0.1f, 0.f, FLT_MAX);
	DragFloat("Range", &aLight.parameters.x, 0.1f, 0.f, FLT_MAX);
	DragFloat3("Attenuation", &aLight.parameters.y, 0.1f, 0.f, FLT_MAX);
}

void ImGui::InspectSpotLight(SpotLight& aLight)
{
	ColorEdit3("Color", &aLight.color.x);
	DragFloat("Intensity", &aLight.color.w, 0.1f, 0.f, FLT_MAX);
	DragFloat("Range", &aLight.parameters.x, 0.1f, 0.f, FLT_MAX);
	DragFloat3("Attenuation", &aLight.parameters.y, 0.1f, 0.f, FLT_MAX);
	DragFloat("Outer Angle", &aLight.outerAngle, 0.01f, 0.f, XM_PIDIV2);
	DragFloat("Inner Angle", &aLight.innerAngle, 0.01f, 0.f, aLight.outerAngle);
}

void ImGui::InspectLight(Light& aLight)
{
	Checkbox("Enabled", &aLight.enabled);

	auto type{ static_cast<int>(aLight.GetType()) };
	Combo("Type", &type, "Directional\0Point\0Spot\0\0");

	switch (aLight.GetType())
	{
	case LightType::Directional:
		InspectDirectionalLight(aLight.GetLight<DirectionalLight>());
		break;
	case LightType::Point:
		InspectPointLight(aLight.GetLight<PointLight>());
		break;
	case LightType::Spot:
		InspectSpotLight(aLight.GetLight<SpotLight>());
		break;
	}
}
