#include "ShaderCommon.hlsli"
#include "PbrFunctions.hlsli"

float4 main(float4 aPixelPosition : SV_Position) : SV_TARGET
{
    uint2 dim;
    GBufferWorldPosition.GetDimensions(dim.x, dim.y);
    
    const float2 uv = aPixelPosition.xy / dim;
    
    const float4 worldPosition  = GBufferWorldPosition.Sample(DefaultSampler, uv);
    const float4 pixelNormal    = GBufferPixelNormal.Sample(DefaultSampler, uv);
    const float4 albedo         = GBufferAlbedo.Sample(DefaultSampler, uv);
    const float4 metalRoughAo   = GBufferMetalRoughAo.Sample(DefaultSampler, uv);
    
    if (!any(pixelNormal))
        discard;
    
    const float3 L = normalize(float3(-1.f, 1.f, 0.f));
    const float3 V = normalize(CameraPosition.xyz - worldPosition.xyz);
    const float3 N = normalize(pixelNormal.xyz);
    
    const float metallic = metalRoughAo.x;
    const float roughness = metalRoughAo.y;
    
    float3 color = PbrShader(L, V, N, albedo.rgb, metallic, roughness);
    color *= 4.0;
    
    return float4(color, 1.0);
}