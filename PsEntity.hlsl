#include "ShaderCommon.hlsli"

float3 Hash3(uint n) // https://www.shadertoy.com/view/llGSzw
{
    n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;
    uint3 k = n * uint3(n, n * 16807U, n * 48271U);
    return float3(k & 0x7fffffffU) / float(0x7fffffff);
}

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    uint entity = GBufferEntity.Load(uint3(pos.x, pos.y, 0));
    if (entity == 0xffffffff)
        return float4(0.0, 0.0, 0.0, 1.0);
    else
        return float4(Hash3(entity), 1.0);
}