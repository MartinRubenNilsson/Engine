#include "ShaderCommon.hlsli"
#include "PbrCommon.hlsli"

static const float MaxReflectionLOD = 4.0;

float4 main(VsOutFullscreen input) : SV_TARGET
{
    const float4 normalDepth = GBufferNormalDepth.Sample(PointSampler, input.uv);
    if (normalDepth.w == FAR_Z)
        return float4(0.0, 0.0, 0.0, 1.0);
    
    const float3 worldPos = UVDepthToWorld(input.uv, normalDepth.w);
    
    const float3 V = normalize(CameraPosition.xyz - worldPos);
    const float3 N = normalize(UnpackNormal(normalDepth.xyz));
    const float3 R = reflect(-V, N);
    
    const float NdV = dot(N, V);
    if (NdV <= 0.0)
        return float4(0.0, 0.0, 0.0, 1.0);
    
    const float3 albedo = GBufferAlbedo.Sample(PointSampler, input.uv).rgb;
    const float4 metalRoughAo = GBufferMetalRoughAo.Sample(PointSampler, input.uv);
    const float metallic = metalRoughAo.x;
    const float roughness = metalRoughAo.y;
    
    const float3 specularColor = lerp(0.04, albedo, metallic); // 0.04 = default dielectric F0
    const float3 diffuseColor = lerp(albedo, 0.0, metallic);
    
    const float3 kS = FresnelSchlickRoughness(NdV, specularColor, roughness);
    const float3 kD = 1.0 - kS;
    
    const float3 irradiance = IrradianceMap.Sample(TrilinearSampler, N).rgb;
    const float3 prefiltered = PrefilteredMap.SampleLevel(TrilinearSampler, R, roughness * MaxReflectionLOD).rgb;
    const float2 integration = IntegrationMap.Sample(TrilinearSampler, float2(NdV, roughness)).xy;
    const float access = AmbientAccessMap.Sample(TrilinearSampler, input.uv).r;
    
    float3 color = 0.0;
    color += kD * irradiance * diffuseColor; // Diffuse
    color += (kS * integration.x + integration.y) * prefiltered; // Specular
    color *= access; // Ambient occlusion
    
    return float4(color, 1.0);
}