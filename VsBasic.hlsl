#include "ShaderCommon.hlsli"

VsOutBasic main(VsInBasic input)
{
    VsOutBasic output;
    output.position  = mul(CameraViewProj, mul(MeshMatrix, float4(input.position, 1.f)));
    output.normal    = normalize(mul((float3x3)MeshMatrixInvTrans, input.normal));
    output.tangent   = normalize(mul((float3x3)MeshMatrix, input.tangent));
    output.bitangent = normalize(mul((float3x3)MeshMatrix, input.bitangent));
    output.uv = input.uv;
    
    return output;
}