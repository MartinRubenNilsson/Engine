#include "ShaderCommon.hlsli"

// Below occlusion parameters use world space units.

static const float OcclusionEpsilon = 0.05;
static const float OcclusionFadeStart = 0.2;
static const float OcclusionFadeEnd = 2.0;
static const float OcclusionRadius = 0.5; 

float OcclusionFunc(float deltaWorldDepth)
{
    float f = step(OcclusionEpsilon, deltaWorldDepth);
    float g = saturate((OcclusionFadeEnd - deltaWorldDepth) / (OcclusionFadeEnd - OcclusionFadeStart));
    return f * g;
}

float main(VsOutFullscreen input) : SV_TARGET
{
    /*
    * Consider the surface geometry being rendered at the current pixel and let:
    * P = its world position,
    * N = its world normal,
    * Q = P + offset, where offset is randomly sampled from the hemisphere (P, N).
    *
    * Next, consider the surface geometry being rendered at the pixel containing Q and let:
    * R = its world position (which may or may not equal Q).
    */
    
    const float4 normalDepthP = GBufferNormalDepth.Sample(NormalDepthSampler, input.uv);
    const float depthP = normalDepthP.w;
    if (depthP == FAR_Z)
        return 1.0; // at far plane -> no occlusion -> full access
    
    const float3 P = UVDepthToWorld(input.uv, depthP);
    const float3 N = normalize(UnpackNormal(normalDepthP.xyz));
    
    const float3 unitVec = GetRandomUnitVec(input.pos.xy);
    
    float occlusion = 0.0;
    
    [unroll]
    for (uint i = 0; i < OFFSET_VECTOR_COUNT; ++i)
    {
        const float3 offset = reflect(OffsetVectors[i].xyz, unitVec);
        const float flip = sign(dot(offset, N));
        
        const float3 Q = P + flip * OcclusionRadius * offset;
        const float2 uv = WorldToUVDepth(Q).xy;

        const float depthR = GBufferNormalDepth.Sample(NormalDepthSampler, uv).w;
        const float3 R = UVDepthToWorld(uv, depthR);
        
        const float deltaWorldDepth = HyperbolicDepthToLinear(depthP) - HyperbolicDepthToLinear(depthR);
        const float weight = saturate(dot(N, normalize(R - P))); // Only points in front of P can occlude it
        
        occlusion += weight * OcclusionFunc(deltaWorldDepth);
    }
    
    occlusion /= OFFSET_VECTOR_COUNT;
    
    float access = 1.0 - occlusion;
    access = pow(access, 4.0); // Sharpen contrast
    
    return access;
}