#include "ShaderCommon.hlsli"

float4 main(Pixel pixel) : SV_TARGET
{
    //const float3 toCamera = normalize(CameraPosition.xyz - pixel.positionWorld.xyz);
    const float3 toLight = float3(0.f, 1.f, 0.f);
    
    const float3 ambient = { 0.1f, 0.1f, 0.1f };
    const float3 diffuse = float3(0.9f, 0.6f, 0.2f) * saturate(dot(toLight, pixel.normalWorld.xyz));
    
    return float4(saturate(ambient + diffuse), 1.f);
}