cbuffer CameraBuffer : register(b0)
{
    float4x4 WorldToClipMatrix;
}

cbuffer ModelBuffer : register(b1)
{
    float4x4 ModelToWorldMatrix;
}

float4 main( float4 pos : POSITION ) : SV_POSITION
{
    return mul(WorldToClipMatrix, mul(ModelToWorldMatrix, pos));
}