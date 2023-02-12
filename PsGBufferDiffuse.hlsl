#include "ShaderCommon.hlsli"

float4 main(float4 aPixelPosition : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferDiffuse.GetDimensions(dim.x, dim.y);
    return GBufferDiffuse.Sample(DefaultSampler, aPixelPosition.xy / dim);
}