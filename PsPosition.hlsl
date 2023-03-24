#include "ShaderCommon.hlsli"

float4 main(VsOutFullscreen input) : SV_TARGET
{
    const float4 normalDepth = GBufferNormalDepth.Sample(PointSampler, input.uv);
    if (normalDepth.w == FAR_Z)
        return float4(0.0, 0.0, 0.0, 1.0);
    
    const float3 worldPos = UVDepthToWorldPos(input.uv, normalDepth.w);
    return float4(worldPos, 1.0);
}