#include "pch.h"
#include "Material.h"

Material::Material(const aiMaterial& aMaterial)
    : myName{ aMaterial.GetName().C_Str() }
{
    aiString diffusePath{};
    if (aMaterial.GetTextureCount(aiTextureType_DIFFUSE) > 0)
        aMaterial.GetTexture(aiTextureType_DIFFUSE, 0, &diffusePath);

    Debug::Println("Fuck");
}
