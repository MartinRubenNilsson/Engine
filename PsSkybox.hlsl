#include "ShaderCommon.hlsli"

float4 main(GsOutSkybox input) : SV_Target
{
    return EnvironmentMap.Sample(TrilinearSampler, input.worldPos);
}