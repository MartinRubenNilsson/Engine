#include "ShaderCommon.hlsli"

Pixel main(Vertex vertex)
{
    Pixel pixel;
    pixel.positionWorld = mul(ModelMatrix, float4(vertex.position, 1.f));
    pixel.positionClip = mul(WorldToClipMatrix, pixel.positionWorld);
    pixel.normalWorld = normalize(mul(ModelMatrixInverseTranspose, float4(vertex.normal, 0.f)));
    return pixel;
}