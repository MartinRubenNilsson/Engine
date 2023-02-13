#include "ShaderCommon.hlsli"

BasicPixel main(BasicVertex aVertex)
{
    BasicPixel pixel;
    pixel.worldPosition = mul(MeshMatrix, float4(aVertex.position, 1.f));
    pixel.pixelPosition = mul(CameraViewProjMatrix, float4(pixel.worldPosition, 1.f));
    pixel.normal    = normalize(mul((float3x3)MeshMatrixInverseTranspose, aVertex.normal));
    pixel.tangent   = normalize(mul((float3x3)MeshMatrix, aVertex.tangent));
    pixel.bitangent = normalize(mul((float3x3)MeshMatrix, aVertex.bitangent));
    pixel.uv = aVertex.uv;
    
    return pixel;
}