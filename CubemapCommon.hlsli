struct VsOutSkybox
{
    float4 pixelPosition : SV_Position;
    float3 localPosition : POSITION;
};

TextureCube EnvironmentMap : register(t0);