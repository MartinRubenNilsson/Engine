#include "ShaderCommon.hlsli"

float4 main(float4 aPixelPosition : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferNormal.GetDimensions(dim.x, dim.y);
    return normalize(GBufferNormal.Sample(PointSampler, aPixelPosition.xy / dim));
}