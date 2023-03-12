// Physics and Math of Shading | SIGGRAPH Courses [https://www.youtube.com/watch?v=j-A0mwsJRmk]
// OpenGL - PBR (physically based rendering) [https://www.youtube.com/watch?v=5p0e7YNONr8]
// Hammon, Earl, Jr., “PBR Diffuse Lighting for GGX+Smith Microsurfaces,” Game Developers Conference, Feb.–Mar. 2017.

#include "Constants.hlsli"

float3 FresnelSchlick(float LdH, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - LdH, 5);
}

float3 FresnelSchlickRoughness(float LdH, float3 F0, float roughness)
{
    return F0 + (max(1.0 - roughness, F0) - F0) * pow(1.0 - LdH, 5);
}

// In below functions, a is the GGX roughness parameter
// and should be remapped as a = roughness * rougness;

float NormalDistributionGGX(float NdH, float a)
{
    float a2 = a * a;
    float denom = 1.0 + NdH * NdH * (a2 - 1.0);
    denom = denom * denom;
    return a2 / max(denom, 0.0001);
}

// Generates a normalized vector H with pdf(H) = D(H) * NdH / (4 * VdH).
// Input param xi should be uniformly distributed on [0, 1] x [0, 1].
float3 ImportanceSampleGGX(float2 xi, float3 N, float roughness)
{
    const float a = roughness * roughness;
    
    const float phi = 2.0 * PI * xi.x;
    const float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
    const float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    
    const float3 H = { cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta };
    
    const float3 up = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    const float3 T = normalize(cross(up, N));
    const float3 B = normalize(cross(N, T));
    
    const float3 output = mul(H, float3x3(T, B, N));
    
    return normalize(output);
}

float GeometrySchlickGGX(float NdV, float roughness)
{
    float k = roughness * roughness * 0.5;
    return NdV / (NdV * (1.0 - k) + k);
}
  
float GeometrySmithGGX(float NdL, float NdV, float roughness)
{
    return GeometrySchlickGGX(NdL, roughness) * GeometrySchlickGGX(NdV, roughness);
}

float GeometrySmithGGXWithDenom(float NdL, float NdV, float a)
{
    return 0.5 / lerp(2.0 * NdL * NdV, NdL + NdV, a);
}

// This value is multiplied by the incoming radiance to get the outgoing radiance.
float3 BrdfDotGGX(float3 L, float3 V, float3 N, float3 aSurfaceColor, float aMetallic, float aRoughness)
{
    const float NdL = dot(N, L);
    const float NdV = dot(N, V);
    
    if (NdL <= 0.0 || NdV <= 0.0)
        return float3(0.0, 0.0, 0.0);
    
    const float3 H = normalize(L + V);
    const float LdV = saturate(dot(L, V));
    const float LdH = saturate(dot(L, H));
    const float NdH = saturate(dot(N, H));
    
    const float3 specularColor = lerp(0.04, aSurfaceColor, aMetallic); // 0.04 = default dielectric F0
    const float3 diffuseColor = lerp(aSurfaceColor, 0.0, aMetallic);
    
    const float a = aRoughness * aRoughness;
    
    const float3 F = FresnelSchlick(LdH, specularColor);
    const float3 D = NormalDistributionGGX(NdH, a);
    const float3 G_denom = GeometrySmithGGXWithDenom(NdL, NdV, a);
    
    const float3 specularBrdf = F * D * G_denom;
    
    const float3 smooth = 1.05 * (1.0 - specularColor) * (1.0 - pow(1.0 - NdL, 5)) * (1.0 - pow(1.0 - NdV, 5));
    const float facing = 0.5 + 0.5 * LdV;
    const float rough = facing * (0.9 - 0.4 * facing) * ((0.5 + NdH) / NdH);
    const float3 single = lerp(smooth, rough, a);
    const float multi = 0.3641 * a;
    
    const float3 diffuseBrdf = (diffuseColor / PI) * (single + diffuseColor * multi);

    return (specularBrdf + diffuseBrdf) * NdL;
}
