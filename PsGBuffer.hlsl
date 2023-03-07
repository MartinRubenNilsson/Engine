#include "ShaderCommon.hlsli"

PsOutGBuffer main(VsOutBasic input)
{
    const float3 T = normalize(input.tangent);
    const float3 B = normalize(input.bitangent);
    const float3 N = normalize(input.normal);
    
    float3 localNormal = MaterialNormal.Sample(TrilinearSampler, input.uv).xyz;
    localNormal.g = 1.0 - localNormal.g; // Convert OpenGL normal to DirectX normal
    localNormal = normalize(UnpackNormal(localNormal));
    
    const float3 worldNormal = normalize(mul(localNormal, float3x3(T, B, N)));
    
    PsOutGBuffer target;
    target.normalDepth = float4(PackNormal(worldNormal), input.position.z);
    target.albedo         = MaterialAlbedo.Sample(TrilinearSampler, input.uv);
    target.metalRoughAo.r = MaterialMetallic.Sample(TrilinearSampler, input.uv).r;
    target.metalRoughAo.g = MaterialRoughness.Sample(TrilinearSampler, input.uv).r;
    target.metalRoughAo.b = MaterialOcclusion.Sample(TrilinearSampler, input.uv).r;
    target.metalRoughAo.a = 0.0;
    target.entity = MeshEntity[0];
    
    return target;
}