#include "ShaderCommon.hlsli"
#include "CubemapCommon.hlsli"

float4 main(GsOutSkybox input) : SV_Target
{
    return EnvironmentMap.Sample(SamplerLinear, input.worldPos);
}