#include "ShaderCommon.hlsli"
#include "Skybox.hlsli"

// https://catlikecoding.com/unity/tutorials/procedural-meshes/cube-sphere/
// https://mathproofs.blogspot.com/2005/07/mapping-cube-to-sphere.html
// https://math.stackexchange.com/questions/118760/can-someone-please-explain-the-cube-to-sphere-mapping-formula-to-me
float3 CubeToSphere(float3 v)
{
    float3 a = v.yzx * v.yzx;
    float3 b = v.zxy * v.zxy;
    return v * sqrt(1 - a / 2 - b / 2 + a * b / 3);
}

float4 main(SkyboxPixel pixel) : SV_Target
{
    return SkyboxCubemap.Sample(DefaultSampler, CubeToSphere(pixel.localPosition));
}