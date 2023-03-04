#include "ShaderCommon.hlsli"
#include "PbrFunctions.hlsli"

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferPixelNormal.GetDimensions(dim.x, dim.y);
    const float2 uv = pos.xy / dim;
    
    const float4 pixelNormal = GBufferPixelNormal.Sample(PointSampler, uv);
    if (!any(pixelNormal))
        return float4(0.0, 0.0, 0.0, 1.0);
    
    const float3 L = normalize(-LightDirection.xyz);
    const float3 V = normalize(CameraPosition.xyz - GetWorldPosition(uv));
    const float3 N = normalize(pixelNormal.xyz * 2.0 - 1.0); // Unpack normals
    
    const float4 albedo = GBufferAlbedo.Sample(PointSampler, uv);
    const float4 metalRoughAo = GBufferMetalRoughAo.Sample(PointSampler, uv);
    
    const float3 brdf = BrdfDotGGX(L, V, N, albedo.rgb, metalRoughAo.x, metalRoughAo.y);
    const float3 color = brdf * LightColor.rgb;
    
    return float4(color, 1.0);
}