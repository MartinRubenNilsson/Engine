#include "ShaderCommon.hlsli"

float4 main(VsOutFullscreen input) : SV_TARGET
{
    return float4(AmbientAccessMap.Sample(PointSampler, input.uv).rrr, 1.0f);
}