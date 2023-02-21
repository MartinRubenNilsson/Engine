#include "ShaderCommon.hlsli"

float4 main(float4 aPixelPosition : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferMetalRoughAo.GetDimensions(dim.x, dim.y);
    return GBufferMetalRoughAo.Sample(SamplerPoint, aPixelPosition.xy / dim);
}