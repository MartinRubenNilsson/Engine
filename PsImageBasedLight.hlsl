#include "ShaderCommon.hlsli"
#include "PbrFunctions.hlsli"

float4 main(VsOutFullscreen input) : SV_TARGET
{
    const float4 normalDepth = GBufferNormalDepth.Sample(PointSampler, input.uv);
    if (normalDepth.w == 1.f)
        return float4(0.0, 0.0, 0.0, 1.0);
    
    const float3 worldPos = UVDepthToWorld(input.uv, normalDepth.w);
    
    const float3 V = normalize(CameraPosition.xyz - worldPos);
    const float3 N = normalize(UnpackNormal(normalDepth.xyz));
    
    const float NdV = dot(N, V);
    if (NdV <= 0.0)
        return float4(0.0, 0.0, 0.0, 1.0);
    
    const float4 albedo = GBufferAlbedo.Sample(PointSampler, input.uv);
    const float4 metalRoughAo = GBufferMetalRoughAo.Sample(PointSampler, input.uv);
    const float metallic = metalRoughAo.x;
    
    const float3 specularColor = lerp(0.04, albedo, metallic); // 0.04 = default dielectric F0
    const float3 diffuseColor = lerp(albedo, 0.0, metallic);
    
    const float3 F = FresnelSchlick(NdV, specularColor);
    const float3 kD = 1.0 - F;
    
    const float3 ambient = IrradianceMap.Sample(TrilinearSampler, N).rgb * diffuseColor * kD;
    
    return float4(ambient, 1.0);
}