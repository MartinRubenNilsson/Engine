#include "ShaderCommon.hlsli"

float4 main(VsOutFullscreen input) : SV_TARGET
{
    uint2 dim;
    GaussianMap.GetDimensions(dim.x, dim.y);
    const float2 uv = input.pos.xy / dim;
    return GaussianMap.Sample(GaussianSampler, uv);
}