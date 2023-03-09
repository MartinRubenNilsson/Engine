#include "ShaderCommon.hlsli"

static const float Epsilon = 0.05;
static const float FadeStart = 0.2;
static const float FadeEnd = 2.0;

float OcclusionFunction(float distZ)
{
    return (distZ > Epsilon) ? saturate((FadeEnd - distZ) / (FadeEnd - FadeStart)) : 0.0;
}

// todo: replace trilinear sampler with NormalDepthSampler

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    uint2 dim;
    SSAOTexture.GetDimensions(dim.x, dim.y);
    const float2 uv = pos.xy / dim;
    
    const float4 normalDepth = GBufferNormalDepth.Sample(TrilinearSampler, uv);
    const float3 N = normalize(UnpackNormal(normalDepth.xyz));
    const float depth = normalDepth.w;
    
    //const float3 pixelWorldPos = UVDepthToWorld(uv, depth);
    
    
    //[unroll]
    //for (uint i = 0; i < 14; ++i)
    //{
    //    const float3 sampleWorldPos; // todo: calulate using offsets
        
    //    float4 sampleClipPos = mul(ViewProj, float4(sampleWorldPos, 1.0));
    //    sampleClipPos /= sampleClipPos.w;
    //    const float2 sampleUV = { sampleClipPos.x * 0.5 + 0.5, 0.5 - sampleClipPos.y * 0.5 };
        
    //    const float sampleDepth = GBufferSSAO.Sample(TrilinearSampler, sampleUV).w;
    //
    
    return float4(normalDepth.xyz, 1.0f);
}