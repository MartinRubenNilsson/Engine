#include "ShaderCommon.hlsli"

static const float OcclusionRadius = 10.0; // in world space
static const float OcclusionEpsilon = 0.05; // in view space
static const float OcclusionFadeStart = 0.2; // in view space
static const float OcclusionFadeEnd = 2.0; // in view space

float OcclusionFunc(float deltaDepth) // deltaDepth is in view space
{
    float f = step(OcclusionEpsilon, deltaDepth);
    float g = saturate((OcclusionFadeEnd - deltaDepth) / (OcclusionFadeEnd - OcclusionFadeStart));
    return f * g;
}

// todo: replace trilinear sampler with NormalDepthSampler

float main(VsOutFullscreen input) : SV_TARGET
{
    // Consider the surface geometry at the current pixel. Then let:
    // P = its world position
    // N = its world normal
    // Q = P + offset, where offset is sampled from a hemisphere pointing in direction N
    
    // Next, consider the surface geometry at the pixel corresponding to Q. Then let:
    // R = its world position (may or may not equal Q)
    
    const float4 normalDepthP = GBufferNormalDepth.Sample(TrilinearSampler, input.uv);
    const float3 N = normalize(UnpackNormal(normalDepthP.xyz));
    const float depthP = normalDepthP.w;
    
    const float3 P = UVDepthToWorld(input.uv, depthP);
    const float3 randUnitVec = GetRandomUnitVec(input.pos.xy);
    
    float totalOcclusion = 0.0;
    
    [unroll]
    for (uint i = 0; i < OFFSET_VECTOR_COUNT; ++i)
    {
        const float3 offset = reflect(OffsetVectors[i].xyz, randUnitVec);
        const float flip = sign(dot(offset, N));
        
        const float3 Q = P + flip * OcclusionRadius * offset;
        const float2 uv = WorldToUVDepth(Q).xy;

        const float depthR = GBufferNormalDepth.Sample(TrilinearSampler, uv).w;
        const float3 R = UVDepthToWorld(uv, depthR);
        
        const float occlusion = OcclusionFunc(depthP - depthR);
        const float weight = saturate(dot(N, normalize(R - P))); // only points in front of P can occlude P
        
        totalOcclusion += occlusion * weight;
    }
    
    totalOcclusion /= OFFSET_VECTOR_COUNT;
    const float access = 1.0 - totalOcclusion;
    
    return saturate(pow(access, 4.0)); // Sharpen contrast
}