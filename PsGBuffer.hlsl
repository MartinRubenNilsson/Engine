#include "ShaderCommon.hlsli"

PsOutGBuffer main(VsOutBasic input)
{
    const float3 T = normalize(input.tangent);
    const float3 B = normalize(input.bitangent);
    const float3 N = normalize(input.normal);
    
    float3 localNormal;
    localNormal.xy = MaterialNormal.Sample(TrilinearSampler, input.uv);
    localNormal.y = 1.0 - localNormal.y; // Convert OpenGL normal to DirectX normal
    localNormal = UnpackNormal(localNormal);
    localNormal.z = sqrt(1.0 - localNormal.x * localNormal.x - localNormal.y * localNormal.y);
    localNormal = normalize(localNormal);
    
    const float3 worldNormal = normalize(mul(localNormal, float3x3(T, B, N)));
    
    PsOutGBuffer target;
    target.normalDepth = float4(PackNormal(worldNormal), input.position.z);
    target.albedo         = MaterialAlbedo.Sample(TrilinearSampler, input.uv);
    target.metalRoughAo.r = MaterialMetallic.Sample(TrilinearSampler, input.uv);
    target.metalRoughAo.g = MaterialRoughness.Sample(TrilinearSampler, input.uv);
    target.metalRoughAo.b = MaterialOcclusion.Sample(TrilinearSampler, input.uv);
    target.metalRoughAo.a = 0.0;
    target.entity = MeshEntity[0];
    
    return target;
}