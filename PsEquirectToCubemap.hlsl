// https://learnopengl.com/PBR/IBL/Diffuse-irradiance

#include "ShaderCommon.hlsli"

Texture2D EquirectangularMap : register(t0); // temporary

float2 SphereToEquirect(float3 v)
{
    return float2(atan2(v.z, v.x) * DIV2PI + 0.5, acos(v.y) * DIVPI);
}

float4 main(GsOutGenCubemap input) : SV_TARGET
{
    float2 uv = SphereToEquirect(normalize(input.worldPos));
    float3 radiance = EquirectangularMap.Sample(TrilinearSampler, uv).rgb;
    return float4(radiance, 1.0);
}