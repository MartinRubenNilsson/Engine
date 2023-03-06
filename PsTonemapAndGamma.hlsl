#include "ShaderCommon.hlsli"

float4 main(float4 pos : SV_Position) : SV_TARGET
{
    uint2 dim;
    LightningBuffer.GetDimensions(dim.x, dim.y);
    const float2 uv = pos.xy / dim;
    
    float3 color = LightningBuffer.Sample(PointSampler, uv).rgb;
    
    color = color / (1.0 + color); // Reinhard tonemapping
    color = pow(color, 1.0 / 2.2); // Gamma correction
	
    return float4(color, 1.0f);
}