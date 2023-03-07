#include "ShaderCommon.hlsli"

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferSSAO.GetDimensions(dim.x, dim.y);
    float2 uv = pos.xy / dim;
    
    const float depth = GBufferSSAO.Sample(PointSampler, uv).w;
    if (depth == 1.f)
        return float4(0.0, 0.0, 0.0, 1.0);
    
    float3 worldPos = UVDepthToWorld(uv, depth);
    
    return float4(worldPos, 1.0);
}