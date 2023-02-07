#include "ShaderCommon.hlsli"
#include "Skybox.hlsli"

float4 main(SkyboxPixel pixel) : SV_Target
{
    return SkyboxCubemap.Sample(DefaultSampler, pixel.localPosition);
}