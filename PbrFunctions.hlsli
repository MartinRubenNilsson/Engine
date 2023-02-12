// Physics and Math of Shading | SIGGRAPH Courses [https://www.youtube.com/watch?v=j-A0mwsJRmk]
// OpenGL - PBR (physically based rendering) [https://www.youtube.com/watch?v=5p0e7YNONr8]
// Hammon, Earl, Jr., “PBR Diffuse Lighting for GGX+Smith Microsurfaces,” Game Developers Conference, Feb.–Mar. 2017.

#define PI 3.1415926538
#define EPSILON 0.0001

float3 FresnelSchlick(float LdH, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - LdH, 5);
}

float GGX_NormalDistribution(float NdH, float alpha)
{
    float alpha2 = alpha * alpha;
    float denom = 1.0 + NdH * NdH * (alpha2 - 1.0);
    denom = PI * denom * denom;
    return alpha2 / max(denom, EPSILON);
}

float GGX_GeometryWithDenominator(float NdL, float NdV, float alpha)
{
    return 0.5 / lerp(2.0 * NdL * NdV, NdL + NdV, alpha);
}

float3 PbrShader(float3 L, float3 V, float3 N, float3 aAlbedo, float aMetallic, float aRoughness)
{
    float3 H = normalize(L + V);
    
    float LdH = max(dot(L, H), 0.0);
    float NdH = max(dot(N, H), 0.0);
    float NdL = max(dot(N, L), EPSILON);
    float NdV = max(dot(N, V), EPSILON);
    
    float3 F0 = lerp(0.04, aAlbedo, aMetallic); // 0.04 approximates the specular color of dielectrics
    float alpha = aRoughness * aRoughness;
    
    float3 F = FresnelSchlick(LdH, F0);
    float3 D = GGX_NormalDistribution(NdH, alpha);
    float3 G = GGX_GeometryWithDenominator(NdL, NdV, alpha);
    
    float3 specular = F * D * G;
    float3 diffuse = (1.0 - F) * (1.0 - aMetallic) * aAlbedo / PI; // (1.0 - aMetallic) ensures that metals have no diffuse color

    return (specular + diffuse) * NdL;
}
