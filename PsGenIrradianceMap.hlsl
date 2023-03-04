#include "ShaderCommon.hlsli"

float3 SphericalToCartesian(float theta, float phi)
{
    return float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
}

float4 main(GsOutGenCubemap input) : SV_TARGET
{
    // https://www.youtube.com/watch?v=gz24lo4mt1o
    
    const float3 N = normalize(input.worldPos);
    const float3 T = cross(float3(0.0, 1.0, 0.0), N);
    const float3 B = cross(N, T);
    
    const float3x3 TBN = { T, B, N };
    
    float3 irradiance = 0.0;
    uint sampleCount = 0;
    
    const float deltaAngle = 0.025;
    for (float theta = 0.0; theta < PIDIV2; theta += deltaAngle)
    {
        for (float phi = 0.0; phi < PI2; phi += deltaAngle)
        {
            float3 sampleDir = mul(SphericalToCartesian(theta, phi), TBN);
            irradiance += EnvironmentMap.Sample(TrilinearSampler, sampleDir).rgb * cos(theta) * sin(theta);
            sampleCount++;
        }
    }
    
    irradiance *= PI / sampleCount;
    
    return float4(irradiance, 1.0);
}