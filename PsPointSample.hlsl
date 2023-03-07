#include "ShaderCommon.hlsli"

Texture2D Texture : register(t0);

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    Texture.GetDimensions(dim.x, dim.y);
    return Texture.Sample(PointSampler, pos.xy / dim);
}