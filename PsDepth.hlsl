#include "ShaderCommon.hlsli"

float4 main(VsOutFullscreen input) : SV_TARGET
{
    float3 depth = GBufferNormalDepth.Sample(PointSampler, input.uv).www;
    return float4(depth, 1.0);
}