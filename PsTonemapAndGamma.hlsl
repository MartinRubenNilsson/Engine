#include "ShaderCommon.hlsli"

float4 main(VsOutFullscreen input) : SV_TARGET
{
    float3 color = LightningTexture.Sample(PointSampler, input.uv).rgb;
    
    color = color / (1.0 + color); // Reinhard tonemapping
    color = pow(color, 1.0 / 2.2); // Gamma correction
	
    return float4(color, 1.0f);
}