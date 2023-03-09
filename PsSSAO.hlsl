#include "ShaderCommon.hlsli"

// Below occlusion parameters use view space units.

static const float OcclusionEpsilon = 0.05;
static const float OcclusionFadeStart = 0.2;
static const float OcclusionFadeEnd = 2.0;
static const float OcclusionRadius = 0.5; 

float OcclusionFunc(float deltaWiewDepth)
{
    float f = step(OcclusionEpsilon, deltaWiewDepth);
    float g = saturate((OcclusionFadeEnd - deltaWiewDepth) / (OcclusionFadeEnd - OcclusionFadeStart));
    return f * g;
}

// todo: replace trilinear sampler with NormalDepthSampler to avoid false occlusions

float main(VsOutFullscreen input) : SV_TARGET
{
    // Consider the surface geometry at the current pixel. Then let:
    // P = its world position
    // N = its world normal
    // Q = P + offset, where offset is sampled from a hemisphere pointing in direction N
    
    // Next, consider the surface geometry at the pixel corresponding to Q. Then let:
    // R = its world position (may or may not equal Q)
    
    const float4 normalDepthP = GBufferNormalDepth.Sample(TrilinearSampler, input.uv);
    const float depthP = normalDepthP.w;
    if (depthP == 1.0)
        return 1.0; // no occlusion == full access
    
    const float3 N = normalize(UnpackNormal(normalDepthP.xyz));
    
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
        
        const float deltaViewDepth = distance(CameraPosition.xyz, P) - distance(CameraPosition.xyz, R);
        const float occlusion = OcclusionFunc(deltaViewDepth);
        const float weight = saturate(dot(N, normalize(R - P))); // only points in front of P can occlude P
        
        totalOcclusion += occlusion * weight;
    }
    
    totalOcclusion /= OFFSET_VECTOR_COUNT;
    const float access = 1.0 - totalOcclusion;
    
    return saturate(pow(access, 4.0)); // Sharpen contrast
}