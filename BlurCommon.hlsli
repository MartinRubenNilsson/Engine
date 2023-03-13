#include "ShaderCommon.hlsli"

static const int BlurRadius = 5;
static const float BlurWeights[11] = { 0.05, 0.05, 0.1, 0.1, 0.1, 0.2, 0.1, 0.1, 0.1, 0.05, 0.05 };

bool IsContinuous(float4 normalDepthA, float4 normalDepthB)
{
    float3 normalA = normalize(UnpackNormal(normalDepthA.xyz));
    float3 normalB = normalize(UnpackNormal(normalDepthB.xyz));
    float depthA = HyperbolicDepthToLinear(normalDepthA.w);
    float depthB = HyperbolicDepthToLinear(normalDepthB.w);
    return dot(normalA, normalB) >= 0.8 && distance(depthA, depthB) <= 0.2;
}

float4 EdgePreservingBlur(float2 uvPerPixel, float2 centerUV)
{
    const float4 centerNormalDepth = GBufferNormalDepth.Sample(NormalDepthSampler, centerUV);
    
    float4 totalColor = BlurWeights[5] * BlurInputTexture.Sample(PointSampler, centerUV);
    float totalWeight = BlurWeights[5];
    
    [unroll]
    for (int i = -BlurRadius; i <= BlurRadius; ++i)
    {
        if (i == 0)
            continue;
        
        const float2 neighUV = centerUV + i * uvPerPixel;
        const float4 neighNormalDepth = GBufferNormalDepth.Sample(NormalDepthSampler, neighUV);
        
        if (!IsContinuous(centerNormalDepth, neighNormalDepth))
            continue;

        totalColor += BlurWeights[i + BlurRadius] * BlurInputTexture.Sample(PointSampler, neighUV);
        totalWeight += BlurWeights[i + BlurRadius];
    }
	
    return totalColor / totalWeight;
}