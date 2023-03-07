#include "ShaderCommon.hlsli"

PsOutGBuffer main(VsOutBasic input)
{
    const float3 T = normalize(input.tangent);
    const float3 B = normalize(input.bitangent);
    const float3 N = normalize(input.normal);
    
    float3 localNormal = MaterialNormal.Sample(TrilinearSampler, input.uv).xyz;
    localNormal.g = 1.0 - localNormal.g; // Account for DirectX's convention for normal textures
    localNormal = normalize(UnpackNormal(localNormal));
    
    const float3 worldNormal = normalize(mul(localNormal, float3x3(T, B, N)));
    const float3 viewNormal = normalize(mul((float3x3)InvTransView, N)); // Note that we use N (the vertex normal)
    
    PsOutGBuffer target;
    target.ssao = float4(PackNormal(viewNormal), input.position.z);
    target.normal = float4(PackNormal(worldNormal), 0.0);
    target.albedo         = MaterialAlbedo.Sample(TrilinearSampler, input.uv);
    target.metalRoughAo.r = MaterialMetallic.Sample(TrilinearSampler, input.uv).r;
    target.metalRoughAo.g = MaterialRoughness.Sample(TrilinearSampler, input.uv).r;
    target.metalRoughAo.b = MaterialOcclusion.Sample(TrilinearSampler, input.uv).r;
    target.metalRoughAo.a = 0.0;
    target.entity = MeshEntity[0];
    
    return target;
}