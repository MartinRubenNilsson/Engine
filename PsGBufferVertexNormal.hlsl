#include "ShaderCommon.hlsli"

float4 main(float4 aPixelPosition : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferVertexNormal.GetDimensions(dim.x, dim.y);
    return normalize(GBufferVertexNormal.Sample(SamplerPoint, aPixelPosition.xy / dim));
}