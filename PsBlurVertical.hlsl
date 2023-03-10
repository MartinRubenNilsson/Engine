#include "BlurCommon.hlsli"

float4 main(VsOutFullscreen input) : SV_TARGET
{
    uint2 dim;
    BlurInputTexture.GetDimensions(dim.x, dim.y);
    const float2 uvPerPixel = { 0.0, 1.0 / dim.y };
    
    return EdgePreservingBlur(uvPerPixel, input.uv);
}