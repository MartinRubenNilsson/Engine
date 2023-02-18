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
    const float3 toLight = LightPosition.xyz - worldPosition.xyz;
    const float toLightLen = length(toLight);
    if (toLightLen > LightParams.x)
        discard;
    
    const float attenDenom = max(0.01, LightParams.y + toLightLen * (LightParams.z + toLightLen * LightParams.w));
    
    const float4 albedo = GBufferAlbedo.Sample(DefaultSampler, uv);
    const float4 metalRoughAo = GBufferMetalRoughAo.Sample(DefaultSampler, uv);
    
    const float3 L = toLight / toLightLen;
    const float3 V = normalize(CameraPosition.xyz - worldPosition.xyz);
    const float3 N = normalize(pixelNormal.xyz * 2.0 - 1.0); // Unpack normals
    
    const float3 brdf = BrdfDotGGX(L, V, N, albedo.rgb, metalRoughAo.x, metalRoughAo.y);
    const float3 color = brdf * LightColor.rgb / attenDenom;
    
    return float4(color, 1.0);
}