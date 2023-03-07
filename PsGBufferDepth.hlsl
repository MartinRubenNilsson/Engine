#include "ShaderCommon.hlsli"

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferSSAO.GetDimensions(dim.x, dim.y);
    return float4(GBufferSSAO.Sample(PointSampler, pos.xy / dim).www, 1.0);
}