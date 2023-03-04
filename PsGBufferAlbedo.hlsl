#include "ShaderCommon.hlsli"

float4 main(float4 aPixelPosition : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferAlbedo.GetDimensions(dim.x, dim.y);
    return GBufferAlbedo.Sample(PointSampler, aPixelPosition.xy / dim);
}