#include "ShaderCommon.hlsli"
#include "PbrFunctions.hlsli"

float4 main(float4 aPixelPosition : SV_Position) : SV_TARGET
{
    uint2 dim;
    GBufferPixelNormal.GetDimensions(dim.x, dim.y);
    const float2 uv = aPixelPosition.xy / dim;
    
    const float4 pixelNormal = GBufferPixelNormal.Sample(DefaultSampler, uv);
    if (!any(pixelNormal))
        discard;
    
    const float4 worldPosition = GBufferWorldPosition.Sample(DefaultSampler, uv);
    const float4 albedo = GBufferAlbedo.Sample(DefaultSampler, uv);
    const float4 metalRoughAo = GBufferMetalRoughAo.Sample(DefaultSampler, uv);
    
    const float3 L = normalize(-LightDirection);
    const float3 V = normalize(CameraPosition.xyz - worldPosition.xyz);
    const float3 N = normalize(pixelNormal.xyz * 2.0 - 1.0); // Unpack normals
    
    const float metallic = metalRoughAo.x;
    const float roughness = metalRoughAo.y;
    
    const float3 color = BrdfDotGGX(L, V, N, albedo.rgb, metallic, roughness) * LightColor.rgb;
    
    return float4(color, 1.0);
}