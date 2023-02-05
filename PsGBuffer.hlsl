#include "ShaderCommon.hlsli"

GBufferTarget main(BasicPixel pixel)
{
    GBufferTarget target;
    target.worldPosition = pixel.worldPosition;
    target.worldNormal = pixel.worldNormal;
    
    return target;
}