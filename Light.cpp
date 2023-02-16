#include "pch.h"
#include "Light.h"

Light::Light(const aiLight& aLight)
{
	std::memcpy(&myColor, &aLight.mColorDiffuse, sizeof(aiColor3D));

	switch (aLight.mType)
	{
	case aiLightSource_DIRECTIONAL:
	{
		DirectionalLight light{};
		std::memcpy(&light.direction, &aLight.mDirection, sizeof(aiVector3D));
		std::memcpy(&light.up, &aLight.mUp, sizeof(aiVector3D));
		myLight = light;
		break;
	}
	case aiLightSource_POINT:
	{
		PointLight light{};
		std::memcpy(&light.position, &aLight.mPosition, sizeof(aiVector3D));
		myLight = light;
		break;
	}
	case aiLightSource_SPOT:
	{
		SpotLight light{};
		std::memcpy(&light.position, &aLight.mPosition, sizeof(aiVector3D));
		std::memcpy(&light.direction, &aLight.mDirection, sizeof(aiVector3D));
		std::memcpy(&light.up, &aLight.mUp, sizeof(aiVector3D));
		//light.innerAngle = 
		myLight = light;
		break;
	}
	default:
		assert(false);
	}
}
