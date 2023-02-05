#include "ShaderCommon.hlsli"

float4 main(float4 aPosition : SV_POSITION) : SV_TARGET
{
    uint2 dimensions;
    GBufferWorldPosition.GetDimensions(dimensions.x, dimensions.y);
    return GBufferWorldPosition.Sample(BasicSampler, aPosition.xy / dimensions);
}