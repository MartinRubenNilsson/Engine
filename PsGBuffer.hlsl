#include "ShaderCommon.hlsli"

//float4 main(BasicPixel pixel) : SV_TARGET
//{
//    const float3 toLight = float3(0.f, 1.f, 0.f);
    
//    const float3 ambient = { 0.3f, 0.3f, 0.3f };
//    const float3 diffuse = float3(0.9f, 0.6f, 0.2f) * saturate(dot(toLight, pixel.normalWorld.xyz));
//    const float3 color = ambient + diffuse;
    
//    return float4(saturate(color), 1.f);
//}

GBufferTarget main(BasicPixel pixel)
{
    //const float3 toLight = float3(0.f, 1.f, 0.f);
    //const float3 ambient = { 0.3f, 0.3f, 0.3f };
    //const float3 diffuse = float3(0.9f, 0.6f, 0.2f) * saturate(dot(toLight, pixel.normalWorld.xyz));
    //const float3 color = ambient + diffuse;
    
    GBufferTarget target;
    target.worldPosition = pixel.worldPosition;
    target.worldNormal = pixel.worldNormal;
    
    return target;
}