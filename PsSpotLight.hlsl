#include "ShaderCommon.hlsli"
#include "PbrFunctions.hlsli"

float4 main(float4 pos : SV_Position) : SV_TARGET
{
    uint2 dim;
    GBufferSSAO.GetDimensions(dim.x, dim.y);
    float2 uv = pos.xy / dim;
    
    const float depth = GBufferSSAO.Sample(PointSampler, uv).w;
    if (depth == 1.f)
        return float4(0.0, 0.0, 0.0, 1.0);
    
    const float3 worldPos = UVDepthToWorld(uv, depth);
    const float3 toLight = LightPosition.xyz - worldPos;
    const float distanceToLight = length(toLight);
    if (distanceToLight > LightParams.x) // if outside range
        return float4(0.0, 0.0, 0.0, 1.0);
    
    const float3 L = toLight / distanceToLight;
    
    const float angleCos = dot(L, normalize(-LightDirection.xyz));
    const float innerAngleCos = cos(LightConeAngles.x);
    const float outerAngleCos = cos(LightConeAngles.y);
    const float angleAttenuation = clamp((angleCos - outerAngleCos) / (innerAngleCos - outerAngleCos), 0.0, 1.0);
    if (angleAttenuation == 0.0)
        return float4(0.0, 0.0, 0.0, 1.0);
    
    const float4 normal = GBufferNormal.Sample(PointSampler, uv);
    
    const float3 V = normalize(CameraPosition.xyz - worldPos);
    const float3 N = normalize(normal.xyz * 2.0 - 1.0); // Unpack normals
    
    const float4 albedo = GBufferAlbedo.Sample(PointSampler, uv);
    const float4 metalRoughAo = GBufferMetalRoughAo.Sample(PointSampler, uv);
    
    const float3 brdf = BrdfDotGGX(L, V, N, albedo.rgb, metalRoughAo.x, metalRoughAo.y);
    const float3 color = brdf * LightColor.rgb * DistanceAttenuation(distanceToLight) * angleAttenuation;
    
    return float4(color, 1.0);
}