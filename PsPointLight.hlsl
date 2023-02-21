#include "ShaderCommon.hlsli"
#include "PbrFunctions.hlsli"

float4 main(float4 aPixelPosition : SV_Position) : SV_TARGET
{
    uint2 dim;
    GBufferWorldPosition.GetDimensions(dim.x, dim.y);
    const float2 uv = aPixelPosition.xy / dim;
    
    const float4 worldPosition = GBufferWorldPosition.Sample(SamplerPoint, uv);
    const float3 toLight = LightPosition.xyz - worldPosition.xyz;
    const float distanceToLight = length(toLight);
    if (distanceToLight > LightParams.x) // if outside range
        return float4(0.0, 0.0, 0.0, 1.0);
    
    const float4 pixelNormal = GBufferPixelNormal.Sample(SamplerPoint, uv);
    if (!any(pixelNormal))
        return float4(0.0, 0.0, 0.0, 1.0);
    
    const float3 L = toLight / distanceToLight;
    const float3 V = normalize(CameraPosition.xyz - worldPosition.xyz);
    const float3 N = normalize(pixelNormal.xyz * 2.0 - 1.0); // Unpack normals
    
    const float4 albedo = GBufferAlbedo.Sample(SamplerPoint, uv);
    const float4 metalRoughAo = GBufferMetalRoughAo.Sample(SamplerPoint, uv);
    
    const float3 brdf = BrdfDotGGX(L, V, N, albedo.rgb, metalRoughAo.x, metalRoughAo.y);
    const float3 color = brdf * LightColor.rgb * DistanceAttenuation(distanceToLight);
    
    return float4(color, 1.0);
}