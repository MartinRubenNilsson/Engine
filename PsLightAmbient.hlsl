#include "ShaderCommon.hlsli"

float4 main(float4 aPixelPosition : SV_Position) : SV_TARGET
{
    float4 worldNormal = SampleGBufferWorldNormal(aPixelPosition);
    if (!any(worldNormal))
        discard;
    
    float4 worldPosition = SampleGBufferWorldPosition(aPixelPosition);
    
    float4 up = { 0, 1, 0, 0 };
    
    float4 ambient = { 0.1f, 0.05f, 0.2f, 0.f };
    float4 diffuse = { 0.5f, 0.7f, 0.2f, 0.f };
    
    float factor = saturate(dot(worldNormal, up));
    
    float4 result = ambient + diffuse * factor;
    result.a = 1.f;
    
    return result;

}