#include "ShaderCommon.hlsli"

float4 main(float4 aPixelPosition : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferWorldPosition.GetDimensions(dim.x, dim.y);
    return GBufferWorldPosition.Sample(SamplerPoint, aPixelPosition.xy / dim);
}