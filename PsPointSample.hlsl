#include "ShaderCommon.hlsli"

Texture2D Texture : register(t0);

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    Texture.GetDimensions(dim.x, dim.y);
    const float2 uv = pos.xy / dim;
    
    return Texture.Sample(PointSampler, uv);
}