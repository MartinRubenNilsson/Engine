#include "ShaderCommon.hlsli"

BasicPixel main(BasicVertex aVertex)
{
    BasicPixel pixel;
    pixel.worldPosition = mul(MeshMatrix, float4(aVertex.position, 1.f));
    pixel.pixelPosition = mul(CameraViewProjMatrix, pixel.worldPosition);
    pixel.worldNormal = normalize(mul(MeshMatrixInverseTranspose, float4(aVertex.normal, 0.f)));
    pixel.uv = aVertex.uv;
    
    return pixel;
}