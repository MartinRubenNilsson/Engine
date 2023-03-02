#include "ShaderCommon.hlsli"
#include "CubemapCommon.hlsli"

float4 main(VsOutSkybox pixel) : SV_Target
{
    return EnvironmentMap.Sample(SamplerLinear, pixel.localPosition);
}