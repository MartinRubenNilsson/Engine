#include "ShaderCommon.hlsli"
#include "PbrCommon.hlsli"
#include "Random.hlsli"

static const uint SampleCount = 1024u;

float2 main(VsOutFullscreen input) : SV_TARGET
{
    // https://learnopengl.com/PBR/IBL/Specular-IBL
    
    const float NdV = input.uv.x;
    const float roughness = input.uv.y;
    
    const float3 N = { 0.0, 0.0, 1.0 }; // Assume normal is pointing straight forward
    const float3 V = { sqrt(1.0 - NdV * NdV), 0.0, NdV };
    
    float scale = 0.0;
    float bias = 0.0;
    
    for (uint i = 0u; i < SampleCount; ++i)
    {
        const float2 xi = Hammersley(i, SampleCount);
        const float3 H = ImportanceSampleGGX(xi, N, roughness);
        const float3 L = normalize(2.0 * dot(V, H) * H - V);

        const float NdL = saturate(L.z);
        const float NdH = saturate(H.z);
        const float VdH = saturate(dot(V, H));
        
        if (NdL <= 0.0)
            continue;
        
        /*
        * Let A := (f(l, v) / F) * NdL = D * G / (4 * NdV),
        * and B := pdf(H) = D * NdH / (4 * VdH).
        * Then G_Vis := A / B = G * VdH / (NdH * NdV).
        */
        
        const float G = GeometrySmithGGX(NdL, NdV, roughness);
        const float G_Vis = G * VdH / (NdH * NdV);
        const float Fc = pow(1.0 - VdH, 5);
        
        scale += (1.0 - Fc) * G_Vis;
        bias += Fc * G_Vis;
    }
    
    scale /= SampleCount;
    bias /= SampleCount;
	
    return float2(scale, bias);
}