cbuffer CameraBuffer : register(b0)
{
    float4x4 WorldToClipMatrix;
}

cbuffer ModelBuffer : register(b1)
{
    float4x4 ModelToWorldMatrix;
}

struct Vertex
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct Pixel
{
    float4 positionClip : SV_POSITION;
    float4 normalWorld : NORMAL;
};