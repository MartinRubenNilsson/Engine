/*
* Shader input/output
*/

struct BasicVertex
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct BasicPixel
{
    float4 pixelPosition : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 worldNormal : NORMAL;
};

struct GBufferTarget
{
    float4 worldPosition : SV_Target0;
    float4 worldNormal : SV_Target1;
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

/*
* Samplers
*/

SamplerState BasicSampler;

/*
* Functions
*/

void SampleGBuffer(float2 aPixelPosition, out float4 aWorldPosition, out float4 aWorldNormal)
{
    uint2 dimensions;
    GBufferWorldPosition.GetDimensions(dimensions.x, dimensions.y);
    float2 uv = aPixelPosition / dimensions;
    
    aWorldPosition = GBufferWorldPosition.Sample(BasicSampler, uv);
    aWorldNormal = GBufferWorldNormal.Sample(BasicSampler, uv);
}