#include "ShaderCommon.hlsli"

float4 main(float4 aPixelPosition : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferBaseColor.GetDimensions(dim.x, dim.y);
    return GBufferBaseColor.Sample(DefaultSampler, aPixelPosition.xy / dim);
}