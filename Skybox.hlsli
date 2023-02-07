struct SkyboxPixel
{
    float4 pixelPosition : SV_Position;
    float3 localPosition : POSITION;
};

TextureCube SkyboxCubemap : register(t0);