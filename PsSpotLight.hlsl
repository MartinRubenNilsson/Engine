#include "ShaderCommon.hlsli"
#include "PbrFunctions.hlsli"

float4 main(VsOutFullscreen input) : SV_TARGET
{
    const float4 normalDepth = GBufferNormalDepth.Sample(PointSampler, input.uv);
    if (normalDepth.w == FAR_Z)
        return float4(0.0, 0.0, 0.0, 1.0);
    
    const float3 worldPos = UVDepthToWorld(input.uv, normalDepth.w);
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
    
    const float3 V = normalize(CameraPosition.xyz - worldPos);
    const float3 N = normalize(UnpackNormal(normalDepth.xyz)); // Unpack normals
    
    const float4 albedo = GBufferAlbedo.Sample(PointSampler, input.uv);
    const float4 metalRoughAo = GBufferMetalRoughAo.Sample(PointSampler, input.uv);
    
    const float3 brdf = BrdfDotGGX(L, V, N, albedo.rgb, metalRoughAo.x, metalRoughAo.y);
    const float3 color = brdf * LightColor.rgb * DistanceAttenuation(distanceToLight) * angleAttenuation;
    
    return float4(color, 1.0);
}