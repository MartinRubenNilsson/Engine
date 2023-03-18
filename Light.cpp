#include "pch.h"
#include "Light.h"
#include "ShaderCommon.h"

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
	{
		Color color{};
		std::memcpy(&color, &aLight.mColorDiffuse, sizeof(aiColor3D));
		SetColor(color);
	}

	switch (aLight.mType)
	{
	case aiLightSource_DIRECTIONAL:
	{
		DirectionalLight light{};
		SetVariant(light);
		break;
	}
	case aiLightSource_POINT:
	{
		PointLight light{};
		light.parameters = { 5.0f, aLight.mAttenuationConstant, aLight.mAttenuationLinear, aLight.mAttenuationQuadratic };
		SetVariant(light);
		break;
	}
	case aiLightSource_SPOT:
	{
		SpotLight light{};
		light.parameters = { 5.0f, aLight.mAttenuationConstant, aLight.mAttenuationLinear, aLight.mAttenuationQuadratic };
		light.innerAngle = aLight.mAngleInnerCone;
		light.outerAngle = aLight.mAngleOuterCone;
		SetVariant(light);
		break;
	}
	default:
		Debug::Println("Warning: Unrecognized light type");
		break;
	}
}

LightType Light::GetType() const
{
	return static_cast<LightType>(myVariant.index());
}

LightBuffer Light::GetBuffer() const
{
	struct Bufferer
	{
		LightBuffer buffer{};

		void operator()(const DirectionalLight&) {}

		void operator()(const PointLight& aLight)
		{
			buffer.parameters = aLight.parameters;
		}

		void operator()(const SpotLight& aLight)
		{
			buffer.parameters = aLight.parameters;
			buffer.coneAngles = { aLight.innerAngle, aLight.outerAngle, 0.f, 0.f };
		}
	};

	Bufferer bufferer{};
	std::visit(bufferer, myVariant);

	Color color{ myColor };
	color.Premultiply();
	bufferer.buffer.color = color;
	
	return bufferer.buffer;
}

void Light::SetColor(const Color& aColor)
{
	myColor = Vector4::Max(aColor, Vector4::Zero);
	Unpremultiply(myColor);
}

Color Light::GetColor() const
{
	return myColor;
}

void Light::SetVariant(const LightVariant& aVariant)
{
	struct Validator
	{
		void operator()(DirectionalLight&) {}

		void operator()(PointLight& aLight)
		{
			aLight.parameters = Vector4::Max(aLight.parameters, Vector4::Zero);
		}

		void operator()(SpotLight& aLight)
		{
			aLight.parameters = Vector4::Max(aLight.parameters, Vector4::Zero);
			aLight.outerAngle = std::clamp(aLight.outerAngle, 0.f, XM_PIDIV2);
			aLight.innerAngle = std::clamp(aLight.innerAngle, 0.f, aLight.outerAngle);
		}
	};

	myVariant = aVariant;
	std::visit(Validator{}, myVariant);
}

const LightVariant& Light::GetVariant() const
{
	return myVariant;
}

/*
* namespace ImGui
*/

void ImGui::Inspect(Light& aLight)
{
	struct Inspector
	{
		void operator()(DirectionalLight&) {}

		void operator()(PointLight& aLight)
		{
			DragFloat("Range", &aLight.parameters.x, 0.1f, 0.f, FLT_MAX);
			DragFloat3("Attenuation", &aLight.parameters.y, 0.1f, 0.f, FLT_MAX);
		}

		void operator()(SpotLight& aLight)
		{
			DragFloat("Range", &aLight.parameters.x, 0.1f, 0.f, FLT_MAX);
			DragFloat3("Attenuation", &aLight.parameters.y, 0.1f, 0.f, FLT_MAX);
			DragFloat("Outer Angle", &aLight.outerAngle, 0.01f, 0.f, XM_PIDIV2);
			DragFloat("Inner Angle", &aLight.innerAngle, 0.01f, 0.f, aLight.outerAngle);
		}
	};

	Checkbox("Enabled", &aLight.enabled);

	{
		int type{ static_cast<int>(aLight.GetVariant().index()) };
		Combo("Type", &type, "Directional\0Point\0Spot\0\0");
	}

	{
		Color color{ aLight.GetColor() };
		ColorEdit3("Color", &color.x);
		DragFloat("Intensity", &color.w, 0.1f, 0.f, FLT_MAX);
		aLight.SetColor(color);
	}

	{
		LightVariant variant{ aLight.GetVariant() };
		std::visit(Inspector{}, variant);
		aLight.SetVariant(variant);
	}
}
