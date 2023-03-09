#include "ShaderCommon.hlsli"

static const int BlurRadius = 5;
static const float BlurWeights[11] = { 0.05, 0.05, 0.1, 0.1, 0.1, 0.2, 0.1, 0.1, 0.1, 0.05, 0.05 };

bool IsContinuous(float4 normalDepthA, float4 normalDepthB)
{
    float3 normalA = UnpackNormal(normalDepthA.xyz);
    float3 normalB = UnpackNormal(normalDepthB.xyz);
    float depthA = normalDepthA.w;
    float depthB = normalDepthB.w;
    return dot(normalA, normalB) >= 0.8 && distance(depthA, depthB) <= 0.2;
    
    // TODO: depths are in clip space, so using 0.2 is wrong!!!
}

float4 main(VsOutFullscreen input) : SV_TARGET
{
    uint2 dim;
    BlurInputTexture.GetDimensions(dim.x, dim.y);
    const float2 uvPerPixel = { 1.0 / dim.x, 0.0 };
    
    const float4 centerNormalDepth = GBufferNormalDepth.Sample(PointSampler, input.uv);
    
    float totalWeight = BlurWeights[5];
    float4 totalColor = totalWeight * BlurInputTexture.Sample(PointSampler, input.uv);
    
    [unroll]
    for (int i = -BlurRadius; i <= BlurRadius; ++i)
    {
        if (i == 0)
            continue;
        
        const float2 neighUV = input.uv + i * uvPerPixel;
        const float4 neighNormalDepth = GBufferNormalDepth.Sample(PointSampler, neighUV);
        
        if (!IsContinuous(centerNormalDepth, neighNormalDepth))
            continue;

        const float weight = BlurWeights[i + BlurRadius];
        const float4 color = weight * BlurInputTexture.Sample(PointSampler, neighUV);
        
        totalWeight += weight;
        totalColor += color;
    }
	
    return totalColor / totalWeight;
}