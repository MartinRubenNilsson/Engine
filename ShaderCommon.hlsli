/*
* Shader input/output
*/

struct BasicVertex
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float2 uv : TEXCOORD;
};

struct BasicPixel
{
    float4 pixelPosition : SV_POSITION;
    float3 worldPosition : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    float2 uv : TEXCOORD;
};

struct GBufferTarget
{
    float4 worldPosition : SV_Target0;
    float4 vertexNormal : SV_Target1;
    float4 pixelNormal : SV_Target2;
    float4 albedo : SV_Target3;
    float4 metalRoughAo : SV_Target4;
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
Texture2D GBufferVertexNormal   : register(t1);
Texture2D GBufferPixelNormal    : register(t2);
Texture2D GBufferAlbedo         : register(t3);
Texture2D GBufferMetalRoughAo   : register(t4);

Texture2D MaterialAlbedo    : register(t10);
Texture2D MaterialNormal    : register(t11);
Texture2D MaterialMetallic  : register(t12);
Texture2D MaterialRoughness : register(t13);
Texture2D MaterialOcclusion : register(t14);

/*
* Samplers
*/

SamplerState DefaultSampler : register(s0);