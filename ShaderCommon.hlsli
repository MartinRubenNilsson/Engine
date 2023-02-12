/*
* Shader input/output
*/

struct BasicVertex
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct BasicPixel
{
    float4 pixelPosition : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 worldNormal : NORMAL;
    float2 uv : TEXCOORD;
};

struct GBufferTarget
{
    float4 worldPosition : SV_Target0;
    float4 worldNormal : SV_Target1;
    float4 baseColor : SV_Target2;
    float4 metalRoughAo : SV_Target3;
};

/*
* Constant buffers
*/

cbuffer CameraBuffer : register(b0)
{
    float4x4 CameraViewProjMatrix;
    float4 CameraPosition;
}

cbuffer MeshBuffer : register(b1)
{
    float4x4 MeshMatrix;
    float4x4 MeshMatrixInverseTranspose;
}

/*
* Textures
*/

Texture2D GBufferWorldPosition  : register(t0);
Texture2D GBufferWorldNormal    : register(t1);
Texture2D GBufferBaseColor        : register(t2);
Texture2D GBufferMetalRoughAo   : register(t3);

Texture2D MaterialBaseColor   : register(t10);
Texture2D MaterialNormal    : register(t11);
Texture2D MaterialMetallic  : register(t12);
Texture2D MaterialRoughness : register(t13);
Texture2D MaterialOcclusion : register(t14);

/*
* Samplers
*/

SamplerState DefaultSampler : register(s0);