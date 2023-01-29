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

float4 main(Vertex aVertex) : SV_POSITION
{
    const float4 position = float4(aVertex.position, 1.f);
    return mul(WorldToClipMatrix, mul(ModelToWorldMatrix, position));
}