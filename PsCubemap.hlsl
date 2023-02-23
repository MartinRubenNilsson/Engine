Texture2D EquirectMap : register(t0);
SamplerState SamplerDefault : register(s0);

float2 SampleSphericalMap(float3 v)
{
    // https://learnopengl.com/PBR/IBL/Diffuse-irradiance
    float2 uv = { atan2(v.z, v.x), asin(v.y) };
    uv *= float2(0.1591, 0.3183); // (1/2pi, 1/pi)
    uv += 0.5;
    return uv;
}

float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
    return EquirectMap.Sample(SamplerDefault, pos.xy / float2(512.0, 512.0));
}