#include "ShaderCommon.hlsli"

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferNormalDepth.GetDimensions(dim.x, dim.y);
    const float2 uv = pos.xy / dim;
    
    const float4 normalDepth = GBufferNormalDepth.Sample(PointSampler, uv);
    if (normalDepth.w == 1.f)
        return float4(0.0, 0.0, 0.0, 1.0);
    
    const float3 worldPos = UVDepthToWorld(uv, normalDepth.w);
    return float4(worldPos, 1.0);
}