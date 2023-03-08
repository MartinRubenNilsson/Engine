#include "ShaderCommon.hlsli"

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GaussianMap.GetDimensions(dim.x, dim.y);
    float2 uv = pos.xy / dim;
    return GaussianMap.Sample(GaussianSampler, uv);
}