// Physics and Math of Shading | SIGGRAPH Courses [https://www.youtube.com/watch?v=j-A0mwsJRmk]
// OpenGL - PBR (physically based rendering) [https://www.youtube.com/watch?v=5p0e7YNONr8]
// Hammon, Earl, Jr., “PBR Diffuse Lighting for GGX+Smith Microsurfaces,” Game Developers Conference, Feb.–Mar. 2017.

#define PI 3.1415926538
#define EPSILON 0.0001

float3 FresnelSchlick(float LdH, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - LdH, 5);
}

float NormalDistributionGGX(float NdH, float alpha)
{
    float alpha2 = alpha * alpha;
    float denom = 1.0 + NdH * NdH * (alpha2 - 1.0);
    denom = PI * denom * denom;
    return alpha2 / max(denom, EPSILON);
}

// Important! Subsumes denominator of full specular BRDF!
float SmithHammonGGX(float NdL, float NdV, float alpha)
{
    return 0.5 / lerp(2.0 * NdL * NdV, NdL + NdV, alpha);
}

// This value is multiplied by the incoming radiance to get the outgoing radiance.
float3 BrdfDotGGX(float3 L, float3 V, float3 N, float3 aSurfaceColor, float aMetallic, float aRoughness)
{
    const float NdL = dot(N, L);
    const float NdV = dot(N, V);
    
    if (NdL <= 0.0 || NdV <= 0.0)
        return float3(0.0, 0.0, 0.0);
    
    const float3 H = normalize(L + V);
    const float LdV = max(dot(L, V), 0.0);
    const float LdH = max(dot(L, H), 0.0);
    const float NdH = max(dot(N, H), 0.0);
    
    const float3 specularColor = lerp(0.04, aSurfaceColor, aMetallic); // 0.04 = default dielectric F0
    const float3 diffuseColor = lerp(aSurfaceColor, 0.0, aMetallic);
    
    const float alphaGGX = aRoughness * aRoughness;
    
    const float3 F = FresnelSchlick(LdH, specularColor);
    const float3 D = NormalDistributionGGX(NdH, alphaGGX);
    const float3 G = SmithHammonGGX(NdL, NdV, alphaGGX);
    
    const float3 specularBrdf = F * D * G;
    
    // ??? smooth should be a vector3?
    const float smooth = 1.05 * (1.0 - specularColor) * (1.0 - pow(1.0 - NdL, 5)) * (1.0 - pow(1.0 - NdV, 5));
    const float facing = 0.5 + 0.5 * LdV;
    const float rough = facing * (0.9 - 0.4 * facing) * ((0.5 + NdH) / NdH);
    const float single = lerp(smooth, rough, alphaGGX);
    const float multi = 0.3641 * alphaGGX;
    
    const float3 diffuseBrdf = (diffuseColor / PI) * (single + diffuseColor * multi);

    return (specularBrdf + diffuseBrdf) * NdL;
}
