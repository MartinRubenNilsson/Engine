#define PI 3.141592654
#define PI2 6.283185307
#define PIDIV2 1.570796327

struct GsOutGenCubemap
{
    float4 pos : SV_Position;
    float3 worldPos : POSITION;
    uint targetIndex : SV_RenderTargetArrayIndex;
};

struct GsOutSkybox
{
    float4 pos : SV_Position;
    float3 worldPos : POSITION;
};

cbuffer CubemapCameraBuffer : register(b10)
{
    float4x4 CubeFaceViewProjs[6];
}

TextureCube EnvironmentMap : register(t0);
TextureCube IrradianceMap : register(t1);