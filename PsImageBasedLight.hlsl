#include "ShaderCommon.hlsli"
#include "PbrFunctions.hlsli"

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferVertexNormal.GetDimensions(dim.x, dim.y);
    const float2 uv = pos.xy / dim;
    
    const float4 vertexNormal = GBufferVertexNormal.Sample(PointSampler, uv);
    if (!any(vertexNormal))
        return float4(0.0, 0.0, 0.0, 1.0);
    
    const float3 V = normalize(CameraPosition.xyz - GetWorldPosition(uv));
    const float3 N = normalize(vertexNormal.xyz * 2.0 - 1.0); // Unpack normals
    
    const float NdV = dot(N, V);
    if (NdV <= 0.0)
        return float4(0.0, 0.0, 0.0, 1.0);
    
    const float4 albedo = GBufferAlbedo.Sample(PointSampler, uv);
    const float4 metalRoughAo = GBufferMetalRoughAo.Sample(PointSampler, uv);
    const float metallic = metalRoughAo.x;
    
    const float3 specularColor = lerp(0.04, albedo, metallic); // 0.04 = default dielectric F0
    const float3 diffuseColor = lerp(albedo, 0.0, metallic);
    
    const float3 F = FresnelSchlick(NdV, specularColor);
    const float3 kD = 1.0 - F;
    
    const float3 ambient = IrradianceMap.Sample(TrilinearSampler, N).rgb * diffuseColor * kD;
    
    return float4(ambient, 1.0);
}