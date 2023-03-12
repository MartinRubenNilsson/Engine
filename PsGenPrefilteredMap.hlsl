#include "ShaderCommon.hlsli"
#include "PbrCommon.hlsli"
#include "Random.hlsli"

static const uint SampleCount = 1024u;

cbuffer PrefilterBuffer : register(b10) // Temporary cbuffer
{
    float4 Roughness; // (roughness, [unused], [unused], [unused])
}

float4 main(GsOutGenCubemap input) : SV_TARGET
{
    const float3 R = normalize(input.worldPos);
    const float3 N = R;
    const float3 V = R;
    
    float3 totalColor = 0.0;
    float totalWeight = 0.0;
    
    for (uint i = 0u; i < SampleCount; ++i)
    {
        const float2 xi = Hammersley(i, SampleCount);
        const float3 H = ImportanceSampleGGX(xi, N, Roughness.x);
        const float3 L = normalize(2.0 * dot(V, H) * H - V);
        
        const float NdL = saturate(dot(N, L));
        
        if (NdL <= 0.0)
            continue;
        
        totalColor += NdL * EnvironmentMap.SampleLevel(TrilinearSampler, L, 0).rgb;
        totalWeight += NdL;
    }
    
    return float4(totalColor / totalWeight, 1.0);
}