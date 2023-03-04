struct GsOut
{
    float4 posClip : SV_POSITION;
    float3 posWorld : POSITION;
    uint index : SV_RenderTargetArrayIndex;
};

Texture2D EnvironmentMap : register(t0);
SamplerState Sampler : register(s0);

float2 SampleSphericalMap(float3 v)
{
    // https://learnopengl.com/PBR/IBL/Diffuse-irradiance
    float2 uv = { atan2(v.z, v.x), asin(v.y) };
    uv *= float2(0.1591, 0.3183); // (1/2pi, 1/pi)
    uv += 0.5;
    return uv;
}

float4 main(GsOut input) : SV_TARGET
{
    return EnvironmentMap.Sample(Sampler, SampleSphericalMap(normalize(input.posWorld)));
}