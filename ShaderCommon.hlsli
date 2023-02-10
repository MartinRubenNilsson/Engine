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
    float4 diffuse : SV_Target2;
};

/*
* Constant buffers
*/

cbuffer CameraBuffer : register(b0)
{
    float4x4 CameraMatrix;
    float4x4 WorldToClipMatrix;
}

cbuffer MeshBuffer : register(b1)
{
    float4x4 MeshMatrix;
    float4x4 MeshMatrixInverseTranspose;
}

/*
* Textures
*/

Texture2D GBufferWorldPosition : register(t0);
Texture2D GBufferWorldNormal : register(t1);
Texture2D GBufferDiffuse : register(t2);

Texture2D MaterialDiffuse : register(t10);

/*
* Samplers
*/

SamplerState DefaultSampler : register(s0);

/*
* GBuffer Functions
*/

float4 SampleGBufferWorldPosition(float4 aPixelPosition)
{
    uint2 dimensions;
    GBufferWorldPosition.GetDimensions(dimensions.x, dimensions.y);
    return GBufferWorldPosition.Sample(DefaultSampler, aPixelPosition.xy / dimensions);
}

float4 SampleGBufferWorldNormal(float4 aPixelPosition)
{
    uint2 dimensions;
    GBufferWorldNormal.GetDimensions(dimensions.x, dimensions.y);
    return normalize(GBufferWorldNormal.Sample(DefaultSampler, aPixelPosition.xy / dimensions));
}

float4 SampleGBufferDiffuse(float4 aPixelPosition)
{
    uint2 dimensions;
    GBufferDiffuse.GetDimensions(dimensions.x, dimensions.y);
    return GBufferDiffuse.Sample(DefaultSampler, aPixelPosition.xy / dimensions);
}