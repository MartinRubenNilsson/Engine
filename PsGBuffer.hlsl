#include "ShaderCommon.hlsli"

GBufferTarget main(BasicPixel pixel)
{
    GBufferTarget target;
    target.worldPosition = pixel.worldPosition;
    target.worldNormal = pixel.worldNormal;
    target.diffuse = MaterialDiffuse.Sample(DefaultSampler, pixel.uv);
    target.metalRoughAo.r = MaterialMetallic.Sample(DefaultSampler, pixel.uv);
    target.metalRoughAo.g = MaterialRoughness.Sample(DefaultSampler, pixel.uv);
    target.metalRoughAo.b = MaterialOcclusion.Sample(DefaultSampler, pixel.uv);
    
    return target;
}