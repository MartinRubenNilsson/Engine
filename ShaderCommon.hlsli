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

struct Vertex
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct Pixel
{
    float4 positionClip : SV_POSITION;
    float4 positionWorld : POSITION;
    float4 normalWorld : NORMAL;
};