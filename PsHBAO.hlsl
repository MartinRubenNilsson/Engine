#include "ShaderCommon.hlsli"

float main(VsOutFullscreen input) : SV_TARGET
{
    /*
    * Consider the surface geometry being rendered at the current pixel and let:
    * P = its world position,
    * N = its world normal.
    */
    
    const float4 normalDepthP = GBufferNormalDepth.Sample(NormalDepthSampler, input.uv);
    const float depthP = normalDepthP.w;
    if (depthP == FAR_Z)
        return 1.0; // at far plane -> no occlusion -> full access
    
    const float3 P = UVDepthToWorld(input.uv, depthP);
    const float3 N = normalize(UnpackNormal(normalDepthP.xyz));
    
    // todo: work on this
	
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}