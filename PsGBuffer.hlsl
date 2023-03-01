#include "ShaderCommon.hlsli"

PsOutGBuffer main(VsOutBasic input)
{
    const float3 T = normalize(input.tangent);
    const float3 B = normalize(input.bitangent);
    const float3 N = normalize(input.normal);
    
    float3 localNormal = MaterialNormal.Sample(SamplerLinear, input.uv).xyz;
    localNormal.g = 1.0 - localNormal.g; // Convert from OpenGL to DirectX
    localNormal = localNormal * 2.0 - 1.0; // Unpack normals
    
    PsOutGBuffer target;
    target.depth = input.position.z;
    target.vertexNormal   = float4(N, 0.0);
    target.pixelNormal    = float4(mul(localNormal, float3x3(T, B, N)), 0.0);
    target.albedo         = MaterialAlbedo.Sample(SamplerLinear, input.uv);
    target.metalRoughAo.r = MaterialMetallic.Sample(SamplerLinear, input.uv).r;
    target.metalRoughAo.g = MaterialRoughness.Sample(SamplerLinear, input.uv).r;
    target.metalRoughAo.b = MaterialOcclusion.Sample(SamplerLinear, input.uv).r;
    target.metalRoughAo.a = 0.0;
    target.entity = MeshEntity[0];
    
    target.vertexNormal.xyz = target.vertexNormal.xyz * 0.5 + 0.5; // Pack normals
    target.pixelNormal.xyz = target.pixelNormal.xyz * 0.5 + 0.5; // Pack normals
    
    return target;
}