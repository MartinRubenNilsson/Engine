#include "ShaderCommon.hlsli"

Pixel main(Vertex vertex)
{
    Pixel pixel;
    pixel.positionClip = mul(WorldToClipMatrix, mul(ModelToWorldMatrix, float4(vertex.position, 1.f)));
    pixel.normalWorld = mul(ModelToWorldMatrix, float4(vertex.normal, 0.f));
    return pixel;
}