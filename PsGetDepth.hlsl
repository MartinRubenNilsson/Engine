#include "ShaderCommon.hlsli"

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    GBufferNormalDepth.GetDimensions(dim.x, dim.y);
    float2 uv = pos.xy / dim;
    
    const float4 normalDepth = GBufferNormalDepth.Sample(PointSampler, uv);
    return float4(normalDepth.www, 1.0);
}