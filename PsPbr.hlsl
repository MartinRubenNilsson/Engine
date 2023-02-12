#include "ShaderCommon.hlsli"
#include "PbrFunctions.hlsli"

float4 main(float4 aPixelPosition : SV_Position) : SV_TARGET
{
    uint2 dim;
    GBufferWorldPosition.GetDimensions(dim.x, dim.y);
    
    const float2 uv = aPixelPosition.xy / dim;
    
    const float4 worldPosition  = GBufferWorldPosition.Sample(DefaultSampler, uv);
    const float4 worldNormal    = GBufferWorldNormal.Sample(DefaultSampler, uv);
    const float4 baseColor      = GBufferBaseColor.Sample(DefaultSampler, uv);
    const float4 metalRoughAo   = GBufferMetalRoughAo.Sample(DefaultSampler, uv);
    
    if (!any(worldNormal))
        discard;
    
    const float3 L = { 0.f, 1.f, 0.f };
    const float3 V = normalize(CameraPosition.xyz - worldPosition.xyz);
    const float3 N = normalize(worldNormal.xyz);
    
    const float3 color = PbrShader(L, V, N, baseColor.rgb, metalRoughAo.x, metalRoughAo.y);
    return float4(color, 1.0);
}