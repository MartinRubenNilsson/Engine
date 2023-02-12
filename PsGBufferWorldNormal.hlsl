#include "ShaderCommon.hlsli"

float4 main(float4 aPixelPosition : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferWorldNormal.GetDimensions(dim.x, dim.y);
    return normalize(GBufferWorldNormal.Sample(DefaultSampler, aPixelPosition.xy / dim)) * 0.5f + 0.5f;
}