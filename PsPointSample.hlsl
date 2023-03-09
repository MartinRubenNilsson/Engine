#include "ShaderCommon.hlsli"

Texture2D Texture : register(t0);

float4 main(VsOutFullscreen input) : SV_TARGET
{
    return Texture.Sample(PointSampler, input.uv);
}