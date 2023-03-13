#include "ShaderCommon.hlsli"

float4 main(VsOutFullscreen input) : SV_TARGET
{
    return float4(GBufferNormalDepth.Sample(PointSampler, input.uv).www, 1.0);
}