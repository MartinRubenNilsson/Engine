#include "ShaderCommon.hlsli"

GBufferTarget main(BasicPixel aPixel)
{
    const float3 T = normalize(aPixel.tangent);
    const float3 B = normalize(aPixel.bitangent);
    const float3 N = normalize(aPixel.normal);
    
    float3 localNormal = MaterialNormal.Sample(SamplerLinear, aPixel.uv).xyz;
    localNormal.g = 1.0 - localNormal.g; // Convert from OpenGL to DirectX
    localNormal = localNormal * 2.0 - 1.0; // Unpack normals
    
    GBufferTarget target;
    target.worldPosition  = float4(aPixel.worldPosition, 0.0);
    target.vertexNormal   = float4(N, 0.0);
    target.pixelNormal    = float4(mul(localNormal, float3x3(T, B, N)), 0.0);
    target.albedo         = MaterialAlbedo.Sample(SamplerLinear, aPixel.uv);
    target.metalRoughAo.r = MaterialMetallic.Sample(SamplerLinear, aPixel.uv).r;
    target.metalRoughAo.g = MaterialRoughness.Sample(SamplerLinear, aPixel.uv).r;
    target.metalRoughAo.b = MaterialOcclusion.Sample(SamplerLinear, aPixel.uv).r;
    target.metalRoughAo.a = 0.0;
    target.entity = MeshEntity[0];
    
    target.vertexNormal.xyz = target.vertexNormal.xyz * 0.5 + 0.5; // Pack normals
    target.pixelNormal.xyz = target.pixelNormal.xyz * 0.5 + 0.5; // Pack normals
    
    return target;
}