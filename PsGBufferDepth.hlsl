#include "ShaderCommon.hlsli"

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferDepth.GetDimensions(dim.x, dim.y);
    return float4(GBufferDepth.Sample(SamplerPoint, pos.xy / dim).rrr, 1.0);
}