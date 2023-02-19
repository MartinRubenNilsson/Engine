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
    uint entity : SV_Target5;
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
    uint MeshEntity[4];
}

cbuffer LightBuffer : register(b2)
{
    float4 LightColor;      // (r, g, b, [unused])
    float4 LightPosition;   // (x, y, z, 1)
    float4 LightDirection;  // (x, y, z, 0)
    float4 LightParams; // (range, constant attn, linear attn, quadratic attn)
    float4 LightConeAngles; // (inner, outer, [unused], [unused])
};

/*
* Textures
*/

Texture2D GBufferWorldPosition  : register(t0);
Texture2D GBufferVertexNormal   : register(t1);
Texture2D GBufferPixelNormal    : register(t2);
Texture2D GBufferAlbedo         : register(t3);
Texture2D GBufferMetalRoughAo   : register(t4);
Texture2D<uint> GBufferEntity   : register(t5);

Texture2D MaterialAlbedo    : register(t10);
Texture2D MaterialNormal    : register(t11);
Texture2D MaterialMetallic  : register(t12);
Texture2D MaterialRoughness : register(t13);
Texture2D MaterialOcclusion : register(t14);

/*
* Samplers
*/

SamplerState DefaultSampler : register(s0);

/*
* Functions
*/

// To be multiplied by outgoing radiance.
float DistanceAttenuation(float aDistanceToLight)
{
    return 1.f / max(0.01, LightParams.y + aDistanceToLight * (LightParams.z + aDistanceToLight * LightParams.w));
}