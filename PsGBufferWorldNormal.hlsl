#include "ShaderCommon.hlsli"

float4 main(float4 aPixelPosition : SV_POSITION) : SV_TARGET
{
    return SampleGBufferWorldNormal(aPixelPosition) * 0.5f + 0.5f;
}