#include "ShaderCommon.hlsli"

Texture2D Texture : register(t0);

float4 main(float4 aPixelPosition : SV_Position) : SV_TARGET
{
    uint2 dim;
    Texture.GetDimensions(dim.x, dim.y);
    
    float3 color = Texture.Sample(PointSampler, aPixelPosition.xy / dim).rgb;
    
    color = color / (1.0 + color); // Reinhard tonemapping
    color = pow(color, 1.0 / 2.2); // Gamma correction
	
    return float4(color, 1.0f);
}