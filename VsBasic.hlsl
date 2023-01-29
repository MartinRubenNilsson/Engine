#include "ShaderCommon.hlsli"

Pixel main(Vertex vertex)
{
    Pixel pixel;
    pixel.positionWorld = mul(ModelToWorldMatrix, float4(vertex.position, 1.f));
    pixel.positionClip = mul(WorldToClipMatrix, pixel.positionWorld);
    pixel.normalWorld = mul(ModelToWorldMatrix, float4(vertex.normal, 0.f));
    return pixel;
}