#include "ShaderCommon.hlsli"

float4 main(float4 aPixelPosition : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferPixelNormal.GetDimensions(dim.x, dim.y);
    return normalize(GBufferPixelNormal.Sample(SamplerPoint, aPixelPosition.xy / dim));
}